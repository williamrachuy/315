#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "decoder.h"

#define ZERO 0
#define SAME 0
#define TRUE 1
#define FALSE 0
#define INIT_ADDR 0x00400000
#define REG_SIZE 32
#define MEM_SIZE 0x00800000
#define WORD_SIZE 4
#define MAX_CHAR 129

typedef struct _mb_hdr {
   char signature[4];
   unsigned int size;
   unsigned int entry;
   unsigned int filler1;
   unsigned char filler2[64 - 16];
} MB_HDR, *MB_HDR_PTR;

typedef struct {
   int execs, memRefs, clocks;
} Stats;

typedef struct {
   unsigned op, rs, rt, rd, shamt, funct, imm, addr;
} Instruction;

enum regIndex{zero, at, v0, v1, a0, a1, a2, a3, t0, t1, t2, t3, t4, t5, t6, t7,
   s0, s1, s2, s3, s4, s5, s6, s7, t8, t9, k0, k1, gp, sp, fp, ra};

const char regName[33][4] = {"$zr", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", 
   "$a3", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$s0", "$s1", 
   "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", 
   "$sp", "$fp", "$ra", "unk"};

MB_HDR mb_hdr;
int fileLoaded = FALSE;
char file[MAX_CHAR];
unsigned pc,
         mem_ptr,
         reg[REG_SIZE],
         mem[MEM_SIZE];

void resetProgCounter(void) {
   pc = INIT_ADDR;
}

void resetRegisters(void) {
   memset(reg, ZERO, REG_SIZE);
}

void resetMemory(void) {
   memset(mem, ZERO, MEM_SIZE);
}

void resetFile(void) {
   fileLoaded = FALSE;
   mem_ptr = INIT_ADDR;
   memset(file, ZERO, MAX_CHAR);
}

void resetCPU(void) {
   resetProgCounter();
   resetFile();
   resetRegisters();
   resetMemory();
}

void loadLoop(FILE *fd) {
   int valid;

   do {
      valid = fread((void *)&mem[mem_ptr],
         WORD_SIZE, 1, fd);
      if (valid)
         mem_ptr += WORD_SIZE;
      else
         break;
   } while (mem_ptr < MEM_SIZE);
}

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
      fprintf(stderr, "\n   %s not a MIPS ASM binary file.\n", file);
      resetCPU();
      return;
   }
}

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

void execTypeR(Instruction iStruct, char *functStr, Stats *stats) {
   unsigned op = iStruct.op,
            rs = iStruct.rs,
            rt = iStruct.rt,
            rd = iStruct.rd,
            shamt = iStruct.shamt,
            funct = iStruct.funct,
            imm = iStruct.imm,
            addr = iStruct.addr;

   if      (strcmp(functStr, "sll")    == SAME) reg[rd] = (unsigned)reg[rt] << shamt;
   else if (strcmp(functStr, "srl")    == SAME) reg[rd] = (unsigned)reg[rt] >> shamt;
   else if (strcmp(functStr, "sra")    == SAME) reg[rd] = (signed)reg[rt] >> shamt;
   else if (strcmp(functStr, "sllv")   == SAME) reg[rd] = (unsigned)reg[rt] << reg[rs];
   else if (strcmp(functStr, "srlv")   == SAME) reg[rd] = (unsigned)reg[rt] << reg[rs];
   else if (strcmp(functStr, "srav")   == SAME) reg[rd] = (signed)reg[rt] >> reg[rs];
   else if (strcmp(functStr, "jr")     == SAME)      pc = reg[rs];
   else if (strcmp(functStr, "jalr")   == SAME){reg[ra] = pc + 4; pc = reg[rs];}
   else if (strcmp(functStr, "add")    == SAME) reg[rd] = (signed)reg[rt] + (signed)reg[rs];
   else if (strcmp(functStr, "addu")   == SAME) reg[rd] = (unsigned)reg[rt] + (unsigned)reg[rs];
   else if (strcmp(functStr, "sub")    == SAME) reg[rd] = (signed)reg[rt] - (signed)reg[rs];
   else if (strcmp(functStr, "subu")   == SAME) reg[rd] = (unsigned)reg[rt] - (unsigned)reg[rs];
   else if (strcmp(functStr, "and")    == SAME) reg[rd] = reg[rt] & reg[rs];
   else if (strcmp(functStr, "or")     == SAME) reg[rd] = reg[rt] | reg[rs];
   else if (strcmp(functStr, "xor")    == SAME) reg[rd] = reg[rt] ^ reg[rs];
   else if (strcmp(functStr, "nor")    == SAME) reg[rd] = ~(reg[rt] | reg[rs]);
   else if (strcmp(functStr, "slt")    == SAME) reg[rd] = ((signed)reg[rs] < (signed)reg[rt]) ? TRUE : FALSE;
   else if (strcmp(functStr, "sltu")   == SAME) reg[rd] = ((unsigned)reg[rs] < (unsigned)reg[rt])? TRUE : FALSE;
}

void execTypeI(Instruction iStruct, char *functStr, Stats *stats) {
   unsigned op = iStruct.op,
            rs = iStruct.rs,
            rt = iStruct.rt,
            rd = iStruct.rd,
            shamt = iStruct.shamt,
            funct = iStruct.funct,
            imm = iStruct.imm,
            addr = iStruct.addr;

   if      (strcmp(functStr, "beq")    == SAME)     {pc = (reg[rs] == reg[rt]) ? (pc + 4 + imm) : pc; stats->clocks--;}
   else if (strcmp(functStr, "bne")    == SAME)     {pc = (reg[rs] != reg[rt]) ? (pc + 4 + imm) : pc; stats->clocks--;}
   else if (strcmp(functStr, "addi")   == SAME) reg[rd] = (signed)reg[rt] + (signed)imm;
   else if (strcmp(functStr, "addiu")  == SAME) reg[rd] = (unsigned)reg[rt] + (unsigned)imm;
   else if (strcmp(functStr, "slti")   == SAME) reg[rd] = ((signed)reg[rs] < (signed)imm) ? TRUE : FALSE;
   else if (strcmp(functStr, "slti")   == SAME) reg[rd] = ((unsigned)reg[rs] < (unsigned)imm) ? TRUE : FALSE;
   else if (strcmp(functStr, "andi")   == SAME) reg[rd] = (signed)reg[rt] & (signed)imm;
   else if (strcmp(functStr, "ori")    == SAME) reg[ra] = (signed)reg[rt] | (signed)imm;
   else if (strcmp(functStr, "xori")   == SAME) reg[rd] = (signed)reg[rt] ^ (signed)imm;
   else if (strcmp(functStr, "lui")    == SAME){reg[rd] = mem[imm] & 0xFFFF0000; stats->clocks++;}
   else if (strcmp(functStr, "lb")     == SAME) reg[rd] = (signed)reg[rt] - (signed)reg[rs];
   else if (strcmp(functStr, "lh")     == SAME) reg[rd] = (unsigned)reg[rt] - (unsigned)reg[rs];
   else if (strcmp(functStr, "lw")     == SAME) reg[rd] = reg[rt] & reg[rs];
   else if (strcmp(functStr, "lbu")    == SAME) reg[rd] = reg[rt] | reg[rs];
   else if (strcmp(functStr, "lhu")    == SAME) reg[rd] = reg[rt] ^ reg[rs];
   else if (strcmp(functStr, "sb")     == SAME) reg[rd] = ~(reg[rt] | reg[rs]);
   else if (strcmp(functStr, "sh")     == SAME) reg[rd] = (signed)reg[rs] < (signed)reg[rt] ? TRUE : FALSE;
   else if (strcmp(functStr, "sw")     == SAME) reg[rd] = (unsigned)reg[rs] < (unsigned)reg[rt] ? TRUE : FALSE;
}

void execInstruction(unsigned instr, Stats *stats) {
   char type, functStr[8];
   Instruction iStruct = makeInstruction(instr);

   getType(iStruct.op, iStruct.funct, &type, functStr);
   if (type == 'F') {
      printf("   Invalid instruction at 0x%08X\n", pc);
   }
   else if (type == 'R') {
      stats->execs += 1;
      stats->clocks += 4;
      execTypeR(iStruct, functStr, stats);
   }
   else if (type == 'I') {
      stats->execs += 1;
      stats->clocks += 4;
      execTypeI(iStruct, functStr, stats);
   }
   else if (type == 'J') {
      stats->execs += 1;
      stats->clocks += 3;
   }
   else {
      printf("   Invalid instruction at 0x%08X\n", pc);
   }
   pc += WORD_SIZE;
}

void printStatistics(Stats stats) {
   printf("\nProgram statistics: \n\n   Execs: %d\n   MemRefs: %d\n   Clocks: %d\n",
      stats.execs, stats.memRefs, stats.clocks);
}

void printRegisters(void) {
   int i;

   printf("\n");
   for (i = 0; i <= ra; i++)
      printf("   [ %02u - %s ]: 0x%08X\n", i, regName[i], reg[i]);
   printf("\n\n");
}

void runFile(void) {
   Stats stats = {0};
   int i;

   resetProgCounter();
   resetRegisters();
   for (i = INIT_ADDR; i < mem_ptr; i += WORD_SIZE) {
      execInstruction(mem[i], &stats);
   }
   printStatistics(stats);
   printRegisters();
}

void decodeFile(void) {
   char retStr[128];
   int i;

   printf("\nDecoded instructions:\n\n");
   for (i = INIT_ADDR; i < mem_ptr; i += WORD_SIZE) {
      printf("   Instruction @ %08X : %08X\n", i, mem[i]);
      strDecoded((unsigned)mem[i], retStr, i);
      printf("   %s\n\n", retStr);
   }
   printf("\n");
}

void printHelp(void) {
   printf("[ load | file <filename> | run | step | decode | help | quit ]\n");
}

int main (const int argc, const char **argv) {
   char cmd[MAX_CHAR];

   resetCPU();
   printf("\n");
   while (feof(stdin) == 0) {
      printf("Enter a command: ");
      scanf("%s", cmd);
      if (strcmp(cmd, "load") == SAME) {
         scanf("%s", file);
         loadMemory();
      }
      else if (strcmp(cmd, "file") == SAME) {
         if (fileLoaded == TRUE)
            printf("\n   \"%s\" is currently loaded.\n", file);
         else
            printf("\n   No file is currently loaded.\n");
      }
      else if (strcmp(cmd, "run") == SAME) {
         if (fileLoaded == TRUE) {
            printf("\n   Running file %s...\n", file);
            runFile();
         }
         else {
            printf("\n   No file is loaded.\n");
         }
      }/*
      else if (strcmp(cmd, "step") == SAME) {

      }*/
      else if (strcmp(cmd, "decode") == SAME) {
         if (fileLoaded == FALSE) {
            printf("\n   No file loaded.\n");
            continue;
         }
         decodeFile();
      }
      else if (strcmp(cmd, "help") == SAME) {
         printHelp();
      }
      else if (strcmp(cmd, "quit") == SAME) {
         break;
      }
      else {
         printf("\n   Invalid command.\n");
      }
      printf("\n");
   }

   exit(EXIT_SUCCESS);
}