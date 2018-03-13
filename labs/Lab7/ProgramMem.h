#ifndef PROGRAMMEM_H
#define PROGRAMMEM_H

// Struct for decoding program file
typedef struct _mb_hdr {
   char signature[4];
   unsigned int size;
   unsigned int entry;
   unsigned int filler1;
   unsigned char filler2[64 - 16];
}

void loadLoop(FILE *fd);
void loadMemory(void);
void resetMemory(void);
void resetFile(void);

extern unsigned mem_ptr;
extern unsigned mem[MEM_SIZE];
extern _mb_hdr MB_HDR; 
extern int fileLoaded;

#endif