#ifndef PROGRAMMEM_H
#define PROGRAMMEM_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ZERO 0
#define SAME 0
#define TRUE 1
#define FALSE 0
#define INIT_ADDR 0x00000000
#define MAX_CHAR 129
#define MEM_SIZE 0x00800000
#define WORD_SIZE 4

// Struct for decoding program file
typedef struct {
   char signature[4];
   unsigned int size;
   unsigned int entry;
   unsigned int filler1;
   unsigned char filler2[64 - 16];
} _mb_hdr;

void loadLoop(FILE *fd);
void loadMemory(void);
void resetMemory(void);
void resetFile(void);

extern unsigned mem_ptr;
extern unsigned mem[MEM_SIZE];
extern _mb_hdr MB_HDR; 
extern int fileLoaded;

#endif