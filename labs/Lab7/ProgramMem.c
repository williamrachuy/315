#include "ProgramMem.h"

char file[MAX_CHAR];
unsigned mem_ptr;
unsigned mem[MEM_SIZE];
_mb_hdr MB_HDR; 
int fileLoaded = FALSE;

// Resets file so a new one can get loaded in
void resetFile(void) {
   fileLoaded = FALSE;
   mem_ptr = INIT_ADDR;
   memset(file, ZERO, sizeof(char) * MAX_CHAR);
}

// Resets program memory, requiring new load
void resetMemory(void) {
   memset(mem, ZERO, sizeof(unsigned) * MEM_SIZE);
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