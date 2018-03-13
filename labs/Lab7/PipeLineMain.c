#include <stdlib.h>
#include <stdio.h>
#include <string.h>


//int fileLoaded = FALSE;
//char file[MAX_CHAR];




// Decode the instructions, doesn't actually run anything. Basically lab 4
void decodeFile(void) {
   char retStr[128];
   unsigned i = mb_hdr.entry;          // Set iterator to first instruction

   printf("\nDecoded instructions:\n\n");
   while (i < mem_ptr) {
      printf("   Instruction @ %08X : %08X\n", i, mem[i]);
      strDecoded((unsigned)mem[i], retStr, i);
      printf("   %s\n\n", retStr);
      i += WORD_SIZE;
   }
   printf("\n");
}

         
// Main loop for execution. Supports a variety of commands, including a few extras
int main (const int argc, const char **argv) {
   char cmd[MAX_CHAR], retStr[128];

   // Reset CPU for first run
   resetCPU();
   printf("\n");

   // While not end of file on input, keep running (so we have an escape sequence)
   while (feof(stdin) == 0) {
      // Prompt for command

      printf("Enter a command: ");
      scanf("%s", cmd);
      
      if (strcmp(cmd, "load") == SAME) {
         // Compare to load input. If so, reset the "system" and load the program into memory

         resetCPU();
         scanf("%s", file);
         loadMemory();
         resetPC();
      }
      else if (strcmp(cmd, "file") == SAME) {
         // Checks if a files is currently loaded
         if (fileLoaded == TRUE)
            printf("\n   \"%s\" is currently loaded.\n", file);
         else
            printf("\n   No file is currently loaded.\n");
      }
      else if (strcmp(cmd, "run") == SAME) {
         // Runs the program until breakpoint or program runs out (in case syscall for HALT not included)
 
            if (fileLoaded == TRUE) {
            printf("\n   Running file %s...\n", file);
            runFile();
            printStatistics();
            printRegisters();
            
            // If the program has run out, reset for another run
            if(pc == mem_ptr){
               resetStats();
               resetRegisters();
               resetPC();
            }
         }
         else {
            printf("\n   No file is loaded.\n");
         }
      }
      else if (strcmp(cmd, "step") == SAME) {
         // Step through the program
         if (fileLoaded == TRUE) {
            if (pc < mem_ptr) {
               // As long as the PC is less than the final value in the memory, continue

               // Print out the current instruction (handy for debugging)
               printf("   Instruction @ %08X : %08X\n", pc, mem[pc]);
               strDecoded((unsigned)mem[pc], retStr, pc);
               printf("   %s\n\n", retStr);
               
               // Execute instruction once and print out stats and registers
               execInstruction(mem[pc]);
               printStatistics();
               printRegisters();
            }
            else {
               // If end of program, print final stats and indicate the end of program
               
               printf("\nEnd of Program. Reverting...\n");
               resetStats();
               resetRegisters();
               resetPC();        
            }
         }
         else {
            printf("\n   No file is loaded.\n");
         }

      }
      else if (strcmp(cmd, "decode") == SAME) {
         // Print out the decoded version of the program, as done in lab 4
         
         if (fileLoaded == TRUE)
            decodeFile();
         else
            printf("\n   No file loaded.\n");
      }
      else if (strcmp(cmd, "reset") == SAME) {
         // reset CPU on command, clearing out program and all register and memory
         
         resetCPU();
      }
      else if (strcmp(cmd, "help") == SAME) {
         // Print out command list for "system"
         
         printHelp();
      }
      else if (strcmp(cmd, "quit") == SAME) {
         break;
      }
      else if (strcmp(cmd, "brkpt") == SAME) {
         // Allows user to enter the PC value to halt a run, so as to debug the program/ "system"
         
         //printf("\n Enter PC address to break at during run: ");
         
         if ((scanf("%hX", &breakpoint) != 1) || (breakpoint % 4 != 0)){
            breakpoint == 0;
            printf("Invalid breakpoint entry!\n");
         }
      }
      else {
         printf("\n   Invalid command.\n");
      }
      printf("\n");
   }

   exit(EXIT_SUCCESS);
}