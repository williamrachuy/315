#include "PipeSupport.h"

#ifndef CPU_H
#define CPU_H

#define WORD_SIZE 4

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
   unsigned effAddress;
   
} Ivalues;

typedef struct{
   unsigned address;
} Jvalues;



typedef struct{
   unsigned PC_curr;
   Instruction dInstr;
   bool active;
   
} IF_ID_basket;


typedef struct{
   unsigned PC_curr;
   Instruction dInstr;
   char[8] functStr;
   char type;
   bool active;
   Rvalues rData;
   Ivalues iData;
   Jvalues jData;
} ID_IE_basket;

typedef struct{
   Instruction dInstr;
   char type;
   bool active;
   bool blocked;   
   Rvalues rData;
   Ivalues iData;
   Jvalues jData;
} IE_MEM_basket;

typedef struct{
   Instruction dInstr;
   char type;
   bool active;
   bool blocked; 
   Rvalues rData;
   Ivalues iData;
   Jvalues jData;   
} MEM_WB_basket;


#endif