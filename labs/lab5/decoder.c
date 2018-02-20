// Decoding file for Lab 5
// CPE 315 Winter 2018
// Cameron Simpson and Will Rachuy

#include <stdio.h>
#include <string.h>
#include "decoder.h"

// Function returns R type function, or "other"
static void getRFunct(unsigned funct, char *functStr) {
   if      (funct == 0x00) strcpy(functStr, "sll");
   else if (funct == 0x02) strcpy(functStr, "srl");
   else if (funct == 0x03) strcpy(functStr, "sra");
   else if (funct == 0x04) strcpy(functStr, "sllv");
   else if (funct == 0x06) strcpy(functStr, "srlv");
   else if (funct == 0x07) strcpy(functStr, "srav");
   else if (funct == 0x08) strcpy(functStr, "jr");
   else if (funct == 0x09) strcpy(functStr, "jalr");
   else if (funct == 0x20) strcpy(functStr, "add");
   else if (funct == 0x21) strcpy(functStr, "addu");
   else if (funct == 0x22) strcpy(functStr, "sub");
   else if (funct == 0x23) strcpy(functStr, "subu");
   else if (funct == 0x24) strcpy(functStr, "and");
   else if (funct == 0x25) strcpy(functStr, "or");
   else if (funct == 0x26) strcpy(functStr, "xor");
   else if (funct == 0x27) strcpy(functStr, "nor");
   else if (funct == 0x2A) strcpy(functStr, "slt");
   else if (funct == 0x2B) strcpy(functStr, "sltu");
   else if (funct == 0x0C) strcpy(functStr, "syscall");
   else strcpy(functStr, "other");
}

// Function returns I type function, if OP code matches
static void getIFunct(unsigned op, char *functStr) {
   if      (op == 0x04) strcpy(functStr, "beq");
   else if (op == 0x05) strcpy(functStr, "bne");
   else if (op == 0x08) strcpy(functStr, "addi");
   else if (op == 0x09) strcpy(functStr, "addiu");
   else if (op == 0x0A) strcpy(functStr, "slti");
   else if (op == 0x0B) strcpy(functStr, "sltiu");
   else if (op == 0x0C) strcpy(functStr, "andi");
   else if (op == 0x0D) strcpy(functStr, "ori");
   else if (op == 0x0E) strcpy(functStr, "xori");
   else if (op == 0x0F) strcpy(functStr, "lui");
   else if (op == 0x20) strcpy(functStr, "lb");
   else if (op == 0x21) strcpy(functStr, "lh");
   else if (op == 0x23) strcpy(functStr, "lw");
   else if (op == 0x24) strcpy(functStr, "lbu");
   else if (op == 0x25) strcpy(functStr, "lhu");
   else if (op == 0x28) strcpy(functStr, "sb");
   else if (op == 0x29) strcpy(functStr, "sh");
   else if (op == 0x2B) strcpy(functStr, "sw");
   else strcpy(functStr, "");
}

// Function returns J type function, if OP code matches
static void getJFunct(unsigned op, char *functStr) {
   if      (op == 0x02) strcpy(functStr, "j");
   else if (op == 0x03) strcpy(functStr, "jal");
   else strcpy(functStr, "");
}

// Get instruction type and function name, unless invalid
static void getType(unsigned op, unsigned funct, char *type, char *functStr) {
   if (op != 0x00){                    // If not a R type, check for I and J
      getIFunct(op, functStr);         // Get I type, sets "" if not I
      *type = 'I';                     // Set I temporarily
   
      if(strcmp(functStr, "") == 0){   // Check if function found
         *type = 'J';                  // If not, set J temporarily
         getJFunct(op, functStr);      // Get J type, sets "" if not J
      }
      
      if(strcmp(functStr, "") == 0){   // If funct is "", neither I, J, or R
         *type = 'F';                  // Set type as F for invalid op code
      }
   }
   else{                               // If op is 0, R type found. "others" included
      *type = 'R';                     // type is R
      getRFunct(funct, functStr);      // Set function, "other" instrucions set ""
   }
}

// Set the names of the registers based on the index
static void getRegStr(char *reg, unsigned val){
   switch (val){
      case 0:
         strcpy(reg, "$zero");
         break;
      case 1:
         strcpy(reg, "$at");
         break;
      case 2:
         strcpy(reg, "$v0");
         break;
      case 3:
         strcpy(reg, "$v1");
         break;
      case 4:
         strcpy(reg, "$a0");
         break;
      case 5:
         strcpy(reg, "$a1");
         break;
      case 6:
         strcpy(reg, "$a2");
         break;
      case 7:
         strcpy(reg, "$a3");
         break;
      case 8:
         strcpy(reg, "$t0");
         break;
      case 9:
         strcpy(reg, "$t1");
         break;
      case 10:
         strcpy(reg, "$t2");
         break;
      case 11:
         strcpy(reg, "$t3");
         break;
      case 12:
         strcpy(reg, "$t4");
         break;
      case 13:
         strcpy(reg, "$t5");
         break;
      case 14:
         strcpy(reg, "$t6");
         break;
      case 15:
         strcpy(reg, "$t7");
         break;
      case 16:
         strcpy(reg, "$s0");
         break;
      case 17:
         strcpy(reg, "$s1");
         break;
      case 18:
         strcpy(reg, "$s2");
         break;
      case 19:
         strcpy(reg, "$s3");
         break;
      case 20:
         strcpy(reg, "$s4");
         break;
      case 21:
         strcpy(reg, "$s5");
         break;
      case 22:
         strcpy(reg, "$s6");
         break;
      case 23:
         strcpy(reg, "$s7");
         break;
      case 24:
         strcpy(reg, "$t8");
         break;
      case 25:
         strcpy(reg, "$t9");
         break;
      case 26:
         strcpy(reg, "$k0");
         break;
      case 27:
         strcpy(reg, "$k1");
         break;
      case 28:
         strcpy(reg, "$gp");
         break;
      case 29:
         strcpy(reg, "$sp");
         break;
      case 30:
         strcpy(reg, "$fp");
         break;
      case 31:
         strcpy(reg, "$ra");
         break;
      default:
         strcpy(reg, "unk");
         break;
   }
      
}

// Returns sign extended value of input
unsigned signExtend(unsigned orig){
   unsigned extended = orig & 0x0000FFFF;
   
   if (orig & 0x8000){                 // If MSB set, extend bits
      extended |= 0xFFFF0000;          // Sign extend
   }
   
   return extended;                    // Return sign extended value
}


// strDecoded takes in instruction, current PC value for branches, and sets a 
// return string that has the pertinent information about the instruction
void strDecoded(unsigned mem, char *retStr, unsigned PC) {
   // Initialize the strings used for characterization
   char functStr[8], shamtStr[3], type, rsStr[6], rdStr[6], rtStr[6], conCat[100];
                                                                    
   unsigned op = (mem >> 26) & 0x3F, rs = (mem >> 21) & 0x1F;        // Split up the bits into the instruction sections
   unsigned rt = (mem >> 16) & 0x1F, rd = (mem >> 11) & 0x1F;
   unsigned shamt = (mem >> 6) & 0x1F, funct = mem & 0x3F;
   unsigned imm = mem & 0xFFFF, addr = mem & 0x03FFFFFF;
   
                                                                     
   getType(op, funct, &type, functStr);                              // Determine the type of instruction

   
   if(type == 'F'){                                                  // If op code not recognized, output invalid instruction
      strcpy(retStr, "Invalid instruction");
   }
   else if (type == 'R' && (strcmp(functStr, "other") == 0           // If type R and "syscall" or "other", don't output the registers
      || strcmp(functStr, "syscall") == 0)){                         // Since they don't have any significance
      sprintf(retStr, "Op: %02X, %c Type (%s)", op, type, functStr); 
   }
   else if (type == 'R') {                                           // If type R, output all information regarding R type instructions
      getRegStr(rsStr, rs);                                          // Convert all the register indexes to names
      getRegStr(rtStr, rt);
      getRegStr(rdStr, rd);
   
      sprintf(retStr, "Op: %02X, %c Type (%s)\nRs=%02X (%s), Rt=%02X (%s), Rd=%02X (%s), Shamt: %02X", 
         op, type, functStr, rs, rsStr, rt, rtStr, rd, rdStr, shamt);
   }
   else if (type == 'I') {                                           // If type I, output information for I types, and extras if branch or load
      getRegStr(rsStr, rs);                                          // Convert rs and rt indexes to names
      getRegStr(rtStr, rt);

      
      sprintf(retStr, "Opcode=0x%02X, %c Type (%s)\n",               // Collect common info for all I types
         op, type, functStr);
      sprintf(conCat, "Rs=%d (%s), Rt=%d (%s), Imm=0x%04X, signext: 0x%08X (%hd)", 
         rs, rsStr, rt, rtStr, imm, signExtend(imm), (short int)(imm));
      strcat(retStr, conCat);
      
      if(op > 0x0E){                                                 // Output for Load/Store I-types 
         sprintf(conCat, "\nEffAddr=R[%s] + 0x%08X",                 // Add effective address for load/store types
            rsStr, signExtend(imm));
         strcat(retStr, conCat);
      }
      else if(op == 0x04 || op == 0x05){                             // Output for branch I-types
         sprintf(conCat, "\nBranchAddr=0x%08X",                      // Add branch address, immediate << 2 + PC + 4   
            (short int)(imm << 2) + (int)PC + 4);
         strcat(retStr, conCat);
      }
   }
   else if (type == 'J') {                                           // Output for J type instructions
      getJFunct(op, functStr);
      sprintf(retStr, "Op=%02X, %c Type (%s)\nJumpAddr=%08X",        // output type and new address being jumped to
         op, type, functStr, addr << 2);
   }
   else
      strcpy(retStr, "Invalid instruction");                         // default, set invalid instruction
}