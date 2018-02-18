#include <stdio.h>
#include <string.h>
#include "decoder.h"

static void getType(unsigned op, char *type) {
   if (op == 0x00)
      *type = 'R';
   else if (
      op == 0x04 || op == 0x05 || op == 0x08 || op == 0x09 || op == 0x0A ||
      op == 0x0B || op == 0x0C || op == 0x0D || op == 0x0E || op == 0x0F ||
      op == 0x20 || op == 0x21 || op == 0x23 || op == 0x24 || op == 0x25 ||
      op == 0x28 || op == 0x29 || op == 0x2B) *type = 'I';
   else if (op == 0x02 || op == 0x03) *type = 'J';
   else *type = 0;
}

static void getFunct(unsigned funct, char *functStr) {
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
   else strcpy(functStr, "");
}

void strDecoded(unsigned mem, char *retStr) {
   char functStr[6], shamtStr[3], type;
   unsigned
      op = (mem >> 26) & 0x1F, rs = (mem >> 21) & 0x1F, rt = (mem >> 16) & 0x1F,
      rd = (mem >> 11) & 0x1F, shamt = (mem >> 6) & 0x1F, funct = mem & 0x3F,
      imm = mem & 0xFFFF, addr = mem & 0x03FFFFFF;
   getType(op, &type);
   getFunct(funct, functStr);
   if (type == 'R' && strcmp(functStr, "") != 0) {
      sprintf(retStr, "Type: %c, Op: %02X, Rs: %02X, Rt: %02X, Rd: %02X, Funct: %s",
         type, op, rs, rt, rd, functStr);
      if (strcmp(functStr, "sll") == 0 || strcmp(functStr, "srl") == 0 || strcmp(functStr, "sra") == 0) {
         strcat(retStr, ", Shamt: ");
         sprintf(shamtStr, "%02X", shamt);
         strcat(retStr, shamtStr);
      }
   }
   else if (type == 'I') {
      sprintf(retStr, "Type: %c, Op: %02X, Rs: %02X, Rt: %02X, Imm: %04X",
         type, op, rs, rt, imm);
   }
   else if (type == 'J') {
      sprintf(retStr, "Type: %c, Op: %02X, Addr: %08X", type, op, addr);
   }
   else
      strcpy(retStr, "Invalid instruction");
}