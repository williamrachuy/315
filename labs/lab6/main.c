#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "decoder.h"

#define ZERO 0
#define SAME 0
#define TRUE 1
#define FALSE 0
#define INIT_ADDR 0x00000000
#define REG_SIZE 32
#define MEM_SIZE 0x00800000
#define WORD_SIZE 4
#define MAX_CHAR 129

// Struct for decoding program file
typedef struct _mb_hdr {
   char signature[4];
   unsigned int size;
   unsigned int entry;
   unsigned int filler1;
   unsigned char filler2[64 - 16];
} MB_HDR, *MB_HDR_PTR;

// Struct for runtime statistics
typedef struct {
   int execs, memRefs, clocks;
} Stats;

// Struct for instruction code
typedef struct {
   unsigned op, rs, rt, rd, shamt, funct, imm, addr;
} Instruction;

// Enumerated type for register lookup
enum regIndex{zero, at, v0, v1, a0, a1, a2, a3, t0, t1, t2, t3, t4, t5, t6, t7,
   s0, s1, s2, s3, s4, s5, s6, s7, t8, t9, k0, k1, gp, sp, fp, ra};

// Register names as strings for easy lookup and printing
const char regName[33][4] = {"$zr", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", 
   "$a3", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$s0", "$s1", 
   "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", 
   "$sp", "$fp", "$ra", "unk"};

// Some global variables used by the "system" for run time   
MB_HDR mb_hdr;
Stats stats = {ZERO};
int fileLoaded = FALSE;
char file[MAX_CHAR];
unsigned pc,
         breakpoint,
         mem_ptr,
         reg[REG_SIZE],
         mem[MEM_SIZE];

// Resets program counter
void resetPC(void) {
   pc = mb_hdr.entry;
}

// Resets register memory
void resetRegisters(void) {
   memset(reg, ZERO, sizeof(unsigned) * REG_SIZE);
}

// Resets program memory, requiring new load
void resetMemory(void) {
   memset(mem, ZERO, sizeof(unsigned) * MEM_SIZE);
}

// Resets file so a new one can get loaded in
void resetFile(void) {
   fileLoaded = FALSE;
   mem_ptr = INIT_ADDR;
   memset(file, ZERO, sizeof(char) * MAX_CHAR);
}

// Resets stats, used for in between runs
void resetStats(void) {
   memset(&stats, ZERO , sizeof(Stats));
}

// Resets all the "system" usage, calls the above functions
void resetCPU(void) {
   resetPC();
   resetFile();
   resetRegisters();
   resetStats();
   resetMemory();
}

// Loads the program data from the file into "system" memory
void loadLoop(FILE *fd) {
   int valid;

   do {
      valid = fread((void *)&mem[mem_ptr],
         WORD_SIZE, 1, fd);
      //if (valid)
         mem_ptr += WORD_SIZE;
      //else
      //   break;
   } while (mem_ptr < MEM_SIZE && mem_ptr < mb_hdr.size);
}

// Manages file, opens and loads the file while checking for errors along the way
void loadMemory(void) {
   FILE *fd;

   if ((fd = fopen(file, "rb")) != NULL) {
      fread((void *)&mb_hdr, sizeof(MB_HDR), 1, fd);
   }
   else {
      fprintf(stderr, "\n   Couldn't load file.\n");
      resetCPU();
      return;
   }
   if (strcmp(mb_hdr.signature, "~MB") == SAME) {
      printf("\n   Loaded \"%s\", program size = %d bytes.\n", file, mb_hdr.size);
      fileLoaded = TRUE;
      loadLoop(fd);
      fclose(fd);
   }
   else {
      fprintf(stderr, "\n   \"%s\" is not a MIPS ASM binary file.\n", file);
      resetCPU();
      return;
   }
}

// Creates the instruction parameters, as used by different instruction types
Instruction makeInstruction(unsigned instr) {
   Instruction iStruct;

   iStruct.op = (instr >> 26) & 0x3F;
   iStruct.rs = (instr >> 21) & 0x1F;
   iStruct.rt = (instr >> 16) & 0x1F;
   iStruct.rd = (instr >> 11) & 0x1F;
   iStruct.shamt = (instr >> 6) & 0x1F;
   iStruct.funct = instr & 0x3F;
   iStruct.imm = instr & 0xFFFF;
   iStruct.addr = instr & 0x03FFFFFF;

   return iStruct;
}

// Returns sign extended value of input
unsigned signExtendHalfWord(unsigned orig){
   unsigned extended = orig & 0x0000FFFF;
   
   if (orig & 0x8000){                 // If MSB set, extend bits
      extended |= 0xFFFF0000;          // Sign extend
   }
   
   return extended;                    // Return sign extended value
}

// Returns sign extended value of input
unsigned signExtendByte(unsigned orig){
   unsigned extended = orig & 0x000000FF;
   
   if (orig & 0x80){                 // If MSB set, extend bits
      extended |= 0xFFFFFF00;          // Sign extend
   }
   
   return extended;                    // Return sign extended value
}

// Executes R type instructions. Each instruction decoded should be self explanatory
void execTypeR(Instruction iStruct, char *functStr) {
   unsigned rs = iStruct.rs,
            rt = iStruct.rt,
            rd = iStruct.rd,
            shamt = iStruct.shamt;

   if      (strcmp(functStr, "sll")    == SAME) reg[rd] =  (unsigned)reg[rt] << shamt;
   else if (strcmp(functStr, "srl")    == SAME) reg[rd] =  (unsigned)reg[rt] >> shamt;
   else if (strcmp(functStr, "sra")    == SAME) reg[rd] =    (signed)reg[rt] >> shamt;
   else if (strcmp(functStr, "sllv")   == SAME) reg[rd] =  (unsigned)reg[rt] << reg[rs];
   else if (strcmp(functStr, "srlv")   == SAME) reg[rd] =  (unsigned)reg[rt] >> reg[rs];
   else if (strcmp(functStr, "srav")   == SAME) reg[rd] =    (signed)reg[rt] >> reg[rs];
   else if (strcmp(functStr, "jr")     == SAME)      pc =  reg[rs] - WORD_SIZE;
   else if (strcmp(functStr, "jalr")   == SAME){reg[ra] =  pc + WORD_SIZE; pc = reg[rs] - WORD_SIZE;}
   else if (strcmp(functStr, "add")    == SAME) reg[rd] =  (signed)  reg[rt] +   (signed)reg[rs];
   else if (strcmp(functStr, "addu")   == SAME) reg[rd] =  (unsigned)reg[rt] + (unsigned)reg[rs];
   else if (strcmp(functStr, "sub")    == SAME) reg[rd] =    (signed)reg[rs] -   (signed)reg[rt];
   else if (strcmp(functStr, "subu")   == SAME) reg[rd] =  (unsigned)reg[rs] - (unsigned)reg[rt];
   else if (strcmp(functStr, "and")    == SAME) reg[rd] =            reg[rt] & reg[rs];
   else if (strcmp(functStr, "or")     == SAME) reg[rd] =            reg[rt] | reg[rs];
   else if (strcmp(functStr, "xor")    == SAME) reg[rd] =            reg[rt] ^ reg[rs];
   else if (strcmp(functStr, "nor")    == SAME) reg[rd] =          ~(reg[rt] | reg[rs]);
   else if (strcmp(functStr, "slt")    == SAME) reg[rd] =   ((signed)reg[rs] <   (signed)reg[rt]) ? TRUE : FALSE;
   else if (strcmp(functStr, "sltu")   == SAME) reg[rd] = ((unsigned)reg[rs] < (unsigned)reg[rt]) ? TRUE : FALSE;
}

// Determines the shift amount to access bytes or half. Memsize is either 2 for halfwords, or 4 for byte memory access
unsigned memAddrAdjust(unsigned memAddress){
   unsigned shiftAmt;
   
   shiftAmt = memAddress % 4;
   memAddress -= shiftAmt;
   return shiftAmt;
}

// Gets the effective 32 bit value from memory, accounting for the 4 byte per address mem storage method.
// Each memory value is located in increments of 4, so returning a value that is not at a address multiple of 4
// requires grabbing a two portions from the adjacent 2 addresses.
// For example, grabbing word at address 0x00000005 requires grabbing upper byte of location 0x00000004, and upper 3 bytes
// from location 0x00000008, and then combining the two values to make an effective word value.
// The project was started using this memory addressing schema based on how the previous lab was setup, and it was easier
// to do this conversion then to change the entire lab to have sequential byte addresses
signed getMemoryValue(unsigned memAddress){
   unsigned lowAddressVal, highAddressVal;
   unsigned memShiftAmt = memAddrAdjust(memAddress);
   
   // If the memory address is a multiple of 4, just return the word at that location
   if(memShiftAmt == 0) return mem[memAddress];
   
   // Lower address value grabs the byte quantity needed and shifts to place into lower portion of the return word
   lowAddressVal = mem[memAddress - memShiftAmt] >> memShiftAmt * 8;

   // Higher address value grabs the byte quantity needed and shifts to place into upper portion of the return word
   highAddressVal = mem[memAddress + (4- memShiftAmt)] << ((4 - memShiftAmt) * 8);
    
   return lowAddressVal | highAddressVal;
}

// Sets memory, like the above function. Confusing for the way we stored memory
void setMemoryValue(unsigned memAddress, signed value, unsigned size){
   unsigned lowAddressVal, highAddressVal;
   unsigned memShiftAmt = memAddrAdjust(memAddress);
   
   if(memShiftAmt == 0) mem[memAddress] = (mem[memAddress]  & (0xFFFFFFFF << size * 8)) | (value & (0xFFFFFFFF >> size * 8));
   else if(size == 1) {mem[memAddress - memShiftAmt] = (mem[memAddress - memShiftAmt] & (0x0000FF00 << ((memShiftAmt - 1) * 8)) | ((value & 0x000000FF) << (memShiftAmt * 8)));}
   else if(size == 2) {
      mem[memAddress - memShiftAmt] = (mem[memAddress - memShiftAmt] & (0x0000FFFF << (memShiftAmt * 8)) | (value & 0x0000FFFF) << (memShiftAmt * 8));
      
      if(memShiftAmt == 3){
         mem[memAddress + memShiftAmt] = (mem[memAddress + memShiftAmt] & 0x000000FF) | (value >> 8 & 0x000000FF);
      }
   }
   else if(size == 4){
      mem[memAddress - memShiftAmt] = (mem[memAddress - memShiftAmt] & (0xFFFFFFFF << (memShiftAmt * 8)) | (value << (memShiftAmt * 8)));
      mem[memAddress + memShiftAmt] = (mem[memAddress + memShiftAmt] & (0xFFFFFFFF >> ((4-memShiftAmt) * 8)) | (value >> ((4-memShiftAmt) * 8)));
   }      
}

// Executes I type instructions. Each instruction decoded should be self explanatory
void execTypeI(Instruction iStruct, char *functStr) {
   unsigned rs = iStruct.rs,
            rt = iStruct.rt,
            imm = iStruct.imm,
            eff = reg[iStruct.rs] + signExtendHalfWord(iStruct.imm),
            memShiftAmt;

   if      (strcmp(functStr, "beq")    == SAME)     {pc =  (reg[rs] == reg[rt]) ? (pc + (short int)(imm << 2)) : pc; stats.clocks--;}
   else if (strcmp(functStr, "bne")    == SAME)     {pc =  (reg[rs] != reg[rt]) ? (pc + (short int)(imm << 2)) : pc; stats.clocks--;}
   else if (strcmp(functStr, "addi")   == SAME) reg[rt] =    (signed)reg[rs] +   signExtendHalfWord(imm);
   else if (strcmp(functStr, "addiu")  == SAME) reg[rt] =  (unsigned)reg[rs] + (unsigned)imm;
   else if (strcmp(functStr, "slti")   == SAME) reg[rt] =   ((signed)reg[rs] <   signExtendHalfWord(imm)) ? TRUE : FALSE;
   else if (strcmp(functStr, "slti")   == SAME) reg[rt] = ((unsigned)reg[rs] < (unsigned)imm) ? TRUE : FALSE;
   else if (strcmp(functStr, "andi")   == SAME) reg[rt] =    (signed)reg[rs] &   imm;
   else if (strcmp(functStr, "ori")    == SAME) reg[rt] =    (signed)reg[rs] |   imm;
   else if (strcmp(functStr, "xori")   == SAME) reg[rt] =    (signed)reg[rs] ^   imm;
   else if (strcmp(functStr, "lui")    == SAME){reg[rt] = (imm << 16) & 0xFFFF0000; stats.clocks++;}
   else if (strcmp(functStr, "lb")     == SAME){reg[rt] = signExtendByte(getMemoryValue(eff) & 0x000000FF); stats.memRefs++; stats.clocks++;}
   else if (strcmp(functStr, "lh")     == SAME){reg[rt] = signExtendHalfWord(getMemoryValue(eff) & 0x0000FFFF); stats.memRefs++; stats.clocks++;}
   else if (strcmp(functStr, "lw")     == SAME){reg[rt] = getMemoryValue(eff); stats.memRefs++; stats.clocks++;}
   else if (strcmp(functStr, "lbu")    == SAME){reg[rt] = (unsigned)((mem[eff & 0xFFFFFFFC] >> (4 *  (eff % 4))) & 0x000000FF); stats.memRefs++; stats.clocks++;}
   else if (strcmp(functStr, "lhu")    == SAME){reg[rt] = (unsigned)((mem[eff & 0xFFFFFFFC + imm] >> (16 * (eff % 2))) & 0x0000FFFF); stats.memRefs++; stats.clocks++;}
   else if (strcmp(functStr, "sb")     == SAME){mem[eff] = reg[rt] & 0x000000FF + signExtendHalfWord(imm); stats.memRefs++;}
   else if (strcmp(functStr, "sh")     == SAME){mem[eff] = reg[rt] & 0x0000FFFF + signExtendHalfWord(imm); stats.memRefs++;}
   else if (strcmp(functStr, "sw")     == SAME){mem[eff] = reg[rt]; stats.memRefs++;}
}

// Executes J type instructions. Each instruction decoded should be self explanatory
void execTypeJ(Instruction iStruct, char *functStr) {
   unsigned addr = iStruct.addr;  
   
   if (strcmp(functStr, "jal")    == SAME){reg[ra] = pc + WORD_SIZE; pc = ((pc & 0xE0000000) | (addr << 2)) - WORD_SIZE;}
   else if (strcmp(functStr, "j")      == SAME)       pc = ((pc & 0xE0000000) | (addr << 2)) - WORD_SIZE;
          
}

// Checks for the type of instruction and calls the appropriate instruction type execution function
void execInstruction(unsigned instr) {
   char type, functStr[8];
   Instruction iStruct = makeInstruction(instr);

   // Get the type of the instruction first
   getType(iStruct.op, iStruct.funct, &type, functStr);
   
   if(instr == 0x00000000){
      // Check for nop
      stats.execs += 1;
      stats.clocks += 3;
   }
   else if(instr == 0x0000000C){
      // syscall. Assumes HALT, other syscalls to be implemented in future. If PC = mem_ptr, the PC is at end of execution
      if(reg[v0] == 0x000A) pc = mem_ptr;                                   
   }
   else if (type == 'F') {
      // Indicates an invalid instruction has been reached, however continues with program execution
      printf("   Invalid instruction at 0x%08X\n", pc);
   }
   else if (type == 'R') {
      // Executes R type instructions. Assumes 4 clock cycles, but adjusts per instruction as needed
      stats.execs += 1;
      stats.clocks += 4;
      execTypeR(iStruct, functStr);
   }
   else if (type == 'I') {
      // Executes I type instructions. Assumes 4 clock cycles, but adjusts per instruction as needed
      stats.execs += 1;
      stats.clocks += 4;
      execTypeI(iStruct, functStr);
   }
   else if (type == 'J') {
      // Executes J type instructions. Assumes 4 clock cycles, but adjusts per instruction as needed
      stats.execs += 1;
      stats.clocks += 3;
      execTypeJ(iStruct, functStr);
   }
   else {
      // Default, shouldn't be reached
      printf("   Invalid instruction at 0x%08X\n", pc);
   }
   
   // Always increment PC. Any instruction that assumes an add of +4 (like branches) doesn't do so, as it is done here
   pc += WORD_SIZE;
}

// Print out them stats
void printStatistics(void) {
   printf("\nProgram statistics: \n\n   Execs: %d\n   MemRefs: %d\n   Clocks: %d\n",
      stats.execs, stats.memRefs, stats.clocks);
}

// Print out them reggies too
void printRegisters(void) {
   int i;

   printf("\n");
   for (i = 0; i <= ra; i++)
      printf("   [ %02u - %s ]: 0x%08X\n", i, regName[i], reg[i]);
   printf("\n\n");
}

// Run the file until end of program or until a breakpoint is met. If currently at a breakpoint, 
// Execute an instruction to get past the breakpoint and then continue with the program
void runFile(void) {
   if(pc == breakpoint){
      execInstruction(mem[pc]);
   }
   
   while (pc < mem_ptr && pc != breakpoint)
      execInstruction(mem[pc]);
}

// Decode the instructions, doesn't actually run anything. Basically lab 4
void decodeFile(void) {
   char retStr[128];
   unsigned i = mb_hdr.entry;          // Set iterator to first instruction

   printf("\nDecoded instructions:\n\n");
   while (i < mem_ptr) {
      printf("   Instruction @ %08X : %08X\n", i, mem[i]);
      strDecoded((unsigned)mem[i], retStr, i);
      printf("   %s\n\n", retStr);
      i += WORD_SIZE;
   }
   printf("\n");
}

// Prints an explanation of the program commands
void printHelp(void) {
   printf("[ load | file <filename> | run | step | decode | reset | help | quit | brkpt <PC address for breakpoint> ]\n");
}

// Main loop for execution. Supports a variety of commands, including a few extras
int main (const int argc, const char **argv) {
   char cmd[MAX_CHAR], retStr[128];

   // Reset CPU for first run
   resetCPU();
   printf("\n");

   // While not end of file on input, keep running (so we have an escape sequence)
   while (feof(stdin) == 0) {
      // Prompt for command

      printf("Enter a command: ");
      scanf("%s", cmd);
      
      if (strcmp(cmd, "load") == SAME) {
         // Compare to load input. If so, reset the "system" and load the program into memory

         resetCPU();
         scanf("%s", file);
         loadMemory();
         resetPC();
      }
      else if (strcmp(cmd, "file") == SAME) {
         // Checks if a files is currently loaded
         if (fileLoaded == TRUE)
            printf("\n   \"%s\" is currently loaded.\n", file);
         else
            printf("\n   No file is currently loaded.\n");
      }
      else if (strcmp(cmd, "run") == SAME) {
         // Runs the program until breakpoint or program runs out (in case syscall for HALT not included)
 
            if (fileLoaded == TRUE) {
            printf("\n   Running file %s...\n", file);
            runFile();
            printStatistics();
            printRegisters();
            
            // If the program has run out, reset for another run
            if(pc == mem_ptr){
               resetStats();
               resetRegisters();
               resetPC();
            }
         }
         else {
            printf("\n   No file is loaded.\n");
         }
      }
      else if (strcmp(cmd, "step") == SAME) {
         // Step through the program
         if (fileLoaded == TRUE) {
            if (pc < mem_ptr) {
               // As long as the PC is less than the final value in the memory, continue

               // Print out the current instruction (handy for debugging)
               printf("   Instruction @ %08X : %08X\n", pc, mem[pc]);
               strDecoded((unsigned)mem[pc], retStr, pc);
               printf("   %s\n\n", retStr);
               
               // Execute instruction once and print out stats and registers
               execInstruction(mem[pc]);
               printStatistics();
               printRegisters();
            }
            else {
               // If end of program, print final stats and indicate the end of program
               
               printf("\nEnd of Program. Reverting...\n");
               resetStats();
               resetRegisters();
               resetPC();        
            }
         }
         else {
            printf("\n   No file is loaded.\n");
         }

      }
      else if (strcmp(cmd, "decode") == SAME) {
         // Print out the decoded version of the program, as done in lab 4
         
         if (fileLoaded == TRUE)
            decodeFile();
         else
            printf("\n   No file loaded.\n");
      }
      else if (strcmp(cmd, "reset") == SAME) {
         // reset CPU on command, clearing out program and all register and memory
         
         resetCPU();
      }
      else if (strcmp(cmd, "help") == SAME) {
         // Print out command list for "system"
         
         printHelp();
      }
      else if (strcmp(cmd, "quit") == SAME) {
         break;
      }
      else if (strcmp(cmd, "brkpt") == SAME) {
         // Allows user to enter the PC value to halt a run, so as to debug the program/ "system"
         
         //printf("\n Enter PC address to break at during run: ");
         
         if ((scanf("%hX", &breakpoint) != 1) || (breakpoint % 4 != 0)){
            breakpoint == 0;
            printf("Invalid breakpoint entry!\n");
         }
      }
      else {
         printf("\n   Invalid command.\n");
      }
      printf("\n");
   }

   exit(EXIT_SUCCESS);
}