#include "PipeSupport.h"

#ifndef CPU_H
#define CPU_H

#define WORD_SIZE 4

// Struct for instruction code
typedef struct {
   unsigned op, rs, rt, rd, shamt, funct, imm, addr;
} Instruction;

typedef struct{
   unsigned rdAddr;
   unsigned rs, rt, rd;
   unsigned shamt;
} Rvalues;

typedef struct{
   unsigned rtAddr;
   unsigned rs, rt;
   unsigned imm;
   unsigned signExImmed;
   
} Ivalues;

typedef struct{
   unsigned address;
} Jvalues;



typedef struct{
   unsigned PC_curr;
   Instruction dInstr;
   int active;
   
} IF_ID_basket;


typedef struct{
   unsigned PC_curr;
   Instruction dInstr;
   char functStr[8];
   char type;
   int active;
   Rvalues rData;
   Ivalues iData;
   Jvalues jData;
} ID_IE_basket;

typedef struct{
   //Instruction dInstr;
   //char type;
   int active;
   int blocked;
   int is_WB;
   char functStr[8];
   unsigned writeBackReg;
   unsigned writeBackValue;
   unsigned memoryAddress;
} IE_MEM_basket;

typedef struct{
   //Instruction dInstr;
   //char type;
   int active;
   int blocked; 
   char functStr[8];  
   unsigned writeBackReg;
   unsigned writeBackValue;
   unsigned memoryAddress;
} MEM_WB_basket;

void ift();
void id();
void ex();
void memory();
void wb();

extern int haltProgram;

#endif