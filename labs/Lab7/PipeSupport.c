#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "PipeSupport.h"

// Register names as strings for easy lookup and printing
const char regName[33][4] = {"$zr", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", 
   "$a3", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$s0", "$s1", 
   "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", 
   "$sp", "$fp", "$ra", "unk"};
   
Stats stats = {ZERO};

unsigned pc;
unsigned breakpoint;
unsigned reg[REG_SIZE];


// Prints an explanation of the program commands
void printHelp(void) {
   printf("[ load | file <filename> | run | step | decode | reset | help | quit | brkpt <PC address for breakpoint> ]\n");
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