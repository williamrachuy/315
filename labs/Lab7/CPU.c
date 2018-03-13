#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "CPU.h"
#include "ProgramMem.h"

ID_IEIF_ID_basket F_D;
ID_IE_basket D_E;
IE_MEM_basket E_M;
MEM_WB_basket M_W;

void if(){
   
   if(pc < mem_ptr){
      F_D.dInstr = makeInstruction(mem[pc]);
   
      F_D.PC_curr = pc;
      
      pc += 4;
      
      F_D.active = true;
      
   }
   else {
      F_D.active = false;
      
   }
}

void id(){
   
   if(F_D.active){
      InstructionCopy(F_D.dInstr, *(D_E.dInstr));
      getType(ID_IE.dInstr.op, ID_IE.dInstr.funct, *(F_D.dInstr.type), *(ID_IE.functStr));
      
      // Indicate that next clock will have an execute operation
      D_E.active = true;      
      
      // Depending on the instruction type, we will fill a different struct for the instruction decoding
      if(type == 'I'){
         D_E.iData.rtAddr = F_D.dInstr.rt;
         D_E.iData.rs = reg[F_D.dInstr.rs];
         D_E.iData.rt = reg[F_D.dInstr.rt];
         D_E.iData.imm = F_D.dInstr.imm;
         D_E.iData.signExImmed = signExtendHalfWord(F_D.dInstr.imm);
      }
      else if(type == 'J'){
         D_E.jData.address = F_D.dInstr.addr; 
      }
      else if(type == 'R'){
         D_E.rData.rdAddr = F_D.dInstr.rd;
         D_E.rData.rs = reg[F_D.dInstr.rs];
         D_E.rData.rt = reg[F_D.dInstr.rt];
         D_E.rData.rd = reg[F_D.dInstr.rd];
         D_E.rData.shamt = F_D.dInstr.shamt;
      }
      else {
         D_E.active = false;
      }
      

   }
   else{
      D_E.active = false;
   }
   
   
}


void ItypeExecute(){

   E_M.writeBackReg = D_E.iData.rtAddr;   
   
   
   if (strcmp(functStr, "beq") == SAME){
      if(reg[rs] == reg[rt]){ 
         pc =  PC_curr + (short int)(imm << 2);
         F_D.active = false;                    // Flush the pipe
         D_E.active = false;                    // Flush the pipe
      }    

      E_M.active = false;                       // Branch has no mem cycle
      E_M.is_WB = false;
   }
   else if (strcmp(functStr, "bne") == SAME){
      if (reg[rs] != reg[rt]){
         pc = PC_curr + (short int)(imm << 2);
         
         F_D.active = FALSE;                    // Flush the pipe
         D_E.active = FALSE;                    // Flush the pipe
      }
      
      E_M.active = FALSE;                    // Branch has no mem cycle
      E_M.is_WB = FALSE;                     // No Writeback state
   }
   else if (strcmp(functStr, "addi")   == SAME){
      // Writeback cycle, no memory access
      E_M.is_WB = TRUE;                      // Need a writeback state
      E_M.active = FALSE;                    // No mem state
      
      E_M.writeBackValue =    D_E.iData.rs +   D_E.iData.signExImmed;
   }
   else if (strcmp(functStr, "addiu")  == SAME){
      E_M.is_WB = TRUE;                      // Need a writeback state
      E_M.active = FALSE;                    // No mem state   
      
      E_M.writeBackValue =  D_E.iData.rs + D_E.iData.imm;
   }
   else if (strcmp(functStr, "slti")   == SAME){
      E_M.is_WB = TRUE;                      // Need a writeback state
      E_M.active = FALSE;                    // No mem state
      
      E_M.writeBackValue =   ((signed)D_E.iData.rs <   (signed)D_E.iData.signExImmed) ? 1 : 0;
   }
   else if (strcmp(functStr, "sltiu")   == SAME){
      E_M.is_WB = TRUE;                      // Need a writeback state
      E_M.active = FALSE;                    // No mem state
      
      E_M.writeBackValue = (D_E.iData.rs < D_E.iData.imm) ? 1 : 0;
   }
   else if (strcmp(functStr, "andi")   == SAME){
      E_M.is_WB = TRUE;                      // Need a writeback state
      E_M.active = FALSE;                    // No mem state
      
      E_M.writeBackValue =    D_E.iData.rs &   D_E.iData.imm;
   }
   else if (strcmp(functStr, "ori")    == SAME){
      E_M.is_WB = TRUE;                      // Need a writeback state
      E_M.active = FALSE;                    // No mem state
      
      E_M.writeBackValue =    D_E.iData.rs |   D_E.iData.imm;
   }
   else if (strcmp(functStr, "xori")   == SAME){
      E_M.is_WB = TRUE;                      // Need a writeback state
      E_M.active = FALSE;                    // No mem state
      
      E_M.writeBackValue =    D_E.iData.rs ^   D_E.iData.imm;
   }
   else if (strcmp(functStr, "lui")    == SAME){
      E_M.is_WB = TRUE;                      // Need a writeback state
      E_M.active = FALSE;                    // Need a mem state   

      E_M.writeBackValue = (imm << 16) & 0xFFFF0000;  
   }
   else if (strcmp(functStr, "lb")     == SAME){
      E_M.is_WB = TRUE;                      // Need a writeback state
      E_M.active = TRUE;                    // Need a mem state  
      
      E_M.memoryAddress = reg[D_E.iData.rs] + D_E.iData.signExImmed;
   }
   else if (strcmp(functStr, "lh")     == SAME){
      E_M.is_WB = TRUE;                      // Need a writeback state
      E_M.active = TRUE;                    // Need a mem state  
      
      E_M.memoryAddress = reg[D_E.iData.rs] + D_E.iData.signExImmed;
   }
   else if (strcmp(functStr, "lw")     == SAME){
      E_M.is_WB = TRUE;                      // Need a writeback state
      E_M.active = TRUE;                    // Need a mem state  
      
      E_M.memoryAddress = reg[D_E.iData.rs] + D_E.iData.signExImmed;
   }
   else if (strcmp(functStr, "lbu")    == SAME){
      E_M.is_WB = TRUE;                      // Need a writeback state
      E_M.active = TRUE;                    // Need a mem state  
      
      E_M.memoryAddress = reg[D_E.iData.rs] + D_E.iData.signExImmed;
   }
   else if (strcmp(functStr, "lhu")    == SAME){
      E_M.is_WB = TRUE;                      // Need a writeback state
      E_M.active = TRUE;                    // Need a mem state  
      
      E_M.memoryAddress = reg[D_E.iData.rs] + D_E.iData.signExImmed;
   }
   else if (strcmp(functStr, "sb")     == SAME){
      E_M.is_WB = TRUE;                      // Need a writeback state
      E_M.active = FALSE;                    // Dont Need a mem state  
      
      E_M.memoryAddress = reg[D_E.iData.rs] + D_E.iData.signExImmed;
      writeBackValue = reg[D_E.iData.rt] & 0x000000FF;
   }
   else if (strcmp(functStr, "sh")     == SAME){
      E_M.is_WB = TRUE;                      // Need a writeback state
      E_M.active = FALSE;                    // Dont Need a mem state  
      
      E_M.memoryAddress = reg[D_E.iData.rs] + D_E.iData.signExImmed;
      writeBackValue = reg[D_E.iData.rt] & 0x0000FFFF;
   }
   else if (strcmp(functStr, "sw")     == SAME){
      E_M.is_WB = TRUE;                      // Need a writeback state
      E_M.active = FALSE;                    // Dont Need a mem state  
      
      E_M.memoryAddress = reg[D_E.iData.rs];
      writeBackValue = reg[D_E.iData.rt];
   }
   
}

void MemCylce(){

   stats.memRefs++;

   if (strcmp(functStr, "lb")     == SAME){     
      M_W.writeBackValue = signExtendByte(getMemoryValue(E_M.memoryAddress) & 0x000000FF); stats.memRefs++; stats.clocks++;
   }
   else if (strcmp(functStr, "lh")     == SAME){      
      M_W.writeBackValue = signExtendHalfWord(getMemoryValue(E_M.memoryAddress) & 0x0000FFFF); stats.memRefs++; stats.clocks++;
   }
   else if (strcmp(functStr, "lw")     == SAME){     
      M_W.writeBackValue = getMemoryValue(E_M.memoryAddress); stats.memRefs++; stats.clocks++;
   }
   else if (strcmp(functStr, "lbu")    == SAME){
      M_W.writeBackValue = (unsigned)((getMemoryValue(E_M.memoryAddress) >> (4 *  (E_M.memoryAddress % 4))) & 0x000000FF); stats.memRefs++; stats.clocks++;
   }
   else if (strcmp(functStr, "lhu")    == SAME){ 
      M_W.writeBackValue = (unsigned)((getMemoryValue(E_M.memoryAddress) >> (16 * (E_M.memoryAddress % 2))) & 0x0000FFFF); stats.memRefs++; stats.clocks++;
   }
}

void writeBack(){
   
   if (strcmp(functStr, "sb")     == SAME){
      stats.memRefs++;
          
      setMemoryValue(E_M.memoryAddress, E_M.writeBackValue, 1);
   }
   else if (strcmp(functStr, "sh")     == SAME){
      stats.memRefs++;
      
      setMemoryValue(E_M.memoryAddress, E_M.writeBackValue, 2);
   }
   else if (strcmp(functStr, "sw")     == SAME){
      stats.memRefs++;
      
      setMemoryValue(E_M.memoryAddress, E_M.writeBackValue, 4);
   }
   else {
      reg[M_W.writeBackReg] = M_W.writeBackValue;
   }
}


void ex(){
   
   InstructionCopy(D_E.dInstr, *(E_M.dInstr));        // Copy the decoded instruction to Exec->Mem basket
   memcpy(D_E.functStr, M_E.functStr, 8);             // Copy the function string to Exec->Mem basket
   
   if(D_E.active){
      E_M.active = true;
      
      if(D_E.type == 'I'){
         ItypeExecute();
      }
      else if(D_E.type == 'J'){
         
      }
      else if(D_E.type == 'R'){
         
      }
      else if(D_E.type == 'F'){
         
      }
   }
   else{
      E_M.active = false;
   }
   
}

void mem(){
   
   memcpy(E_M.functStr, M_W.functStr, 8);             // Copy the function string to Mem->WB basket
   
   if(E_M.active){
      MemCylce();
   }
   
   if(E_M.is_WB){
      M_W.active = TRUE;
      M_W.writeBackReg = E_M.writeBackReg;
      M_W.writeBackValue = E_M.writeBackValue;
      M_W.memoryAddress = E_M.memoryAddress;
   }
   else{
      M_W.active = FALSE;
   }
   
}

void wb(){
 
if(M_W.active){ 
      writeBack();
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