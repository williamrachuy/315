/*------------------------------------------------------------------------------
 * Example mips_asm program loader. This loads the mips_asm binary
 * named "testcase1.mb" into an array in memory. It reads
 * the 64-byte header, then loads the code into the mem array
 *
 * DLR 4/18/16                   
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mips_asm_header.h"
#include "decoder.h"

typedef unsigned int MIPS, *MIPS_PTR;

MB_HDR mb_hdr;                                                                  /* Header area */
MIPS mem[1024];                                                                 /* Room for 4K bytes */

int main(void) {
   FILE *fd;
   int memPtr, i, n;
   char retStr[64], filename[] = "testcase1.mb";

   if ((fd = fopen(filename, "rb")) == NULL) {                                  /* format the MIPS Binary header */
     printf("\nCouldn't load test case - quitting.\n");
     exit(99);
   }
   memPtr = 0;                                                                  /* This is the memory pointer, a byte offset */
   fread((void *)&mb_hdr, sizeof(mb_hdr), 1, fd);                               /* read the header and verify it. */
   if (!strcmp(mb_hdr.signature, "~MB") == 0) {
      printf("\nThis isn't really a mips_asm binary file - quitting.\n");
      exit(98);
   }
   printf("\n%s Loaded ok, program size = %d bytes.\n\n",
      filename, mb_hdr.size);
   do {                                                                         /* read the binary code a word at a time */
      n = fread((void *)&mem[memPtr / 4], 4, 1, fd);                            /* note div/4 to make word index */
      if (n)
         memPtr += 4;                                                           /* Increment byte pointer by size of instr */
      else
         break;
   } while (memPtr < sizeof(mem)) ;
   fclose(fd);
   for (i = 0; i < memPtr; i += 4) {                                            /* ok, now dump out the instructions loaded: */
      printf("Instruction @ %08X : %08X\n", i, mem[i / 4]);                     /* i contains byte offset addresses */
      strDecoded((unsigned)mem[i / 4], retStr);
      printf("Decoded: %s\n\n", retStr);
   }
   printf("\n");

   exit(EXIT_SUCCESS);
}