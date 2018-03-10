#include "PipeSupport.h"

#ifndef CPU_H
#define CPU_H

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
   
} ID_IE_basket;

typedef struct{
   Instruction dInstr;
   char type;
   bool active;
   bool blocked;   
} IE_MEM_basket;

typedef struct{
   Instruction dInstr;
   char type;
   bool active;
   bool blocked;   
} MEM_WB_basket;


#endif