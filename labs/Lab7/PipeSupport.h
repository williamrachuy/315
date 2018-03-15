#ifndef PIPESUPPORT_H
#define PIPESUPPORT_H

#define ZERO 0
#define SAME 0
#define TRUE 1
#define FALSE 0
#define REG_SIZE 32

// Struct for runtime statistics
typedef struct {
   int execs, memRefs, clocks;
} Stats;

// Enumerated type for register lookup
enum regIndex{zero, at, v0, v1, a0, a1, a2, a3, t0, t1, t2, t3, t4, t5, t6, t7,
   s0, s1, s2, s3, s4, s5, s6, s7, t8, t9, k0, k1, gp, sp, fp, ra};

void printHelp(void);   
void printStatistics(void);
void printRegisters(void);
void resetPC(void);   
void resetRegisters(void);
void resetStats(void);   
void resetCPU(void);
   
   
extern Stats stats;
extern unsigned pc;
extern unsigned breakpoint;
extern unsigned reg[REG_SIZE];
extern const char regName[33][4];

#endif