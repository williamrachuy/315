/* CPE 315 LAB 1 */

#include <stdio.h>

void lineBreak(int a, int b) {
   int i;

   for (i = 0; i < a; i++)
      printf("\n");
   printf("========================\n");
   for (i = 0; i < b; i++)
      printf("\n");
}

unsigned char addUnsignedChar(unsigned char a, unsigned char b, unsigned int c) {
   return a + b + c;
}

unsigned int checkCarry(unsigned char a, unsigned char b, unsigned int c) {
   if ((unsigned char)(a + b + c) < a)
      return 1;
   else
      return 0;
}

unsigned int checkOverflow(unsigned char a, unsigned char b) {
   if ((a >> 7 == b >> 7) && ((unsigned char)(a + b) >> 7 != a >> 7))
      return 1;
   else
      return 0;
}

unsigned int arbitraryByteAdd(unsigned char *result, unsigned char *a1, unsigned char *a2, int size, unsigned int carry_in) {
   unsigned int i, carry = carry_in;

   for (i = 0; i < size; i++) {
      result[size-i-1] = addUnsignedChar(a1[size-i-1], a2[size-i-1], carry);
      carry = checkCarry(a1[size-i-1], a2[size-i-1], carry);
   }

   return carry;
}

int main(void) {
   unsigned int i, bytestring_r_carry, size = 5;

   unsigned char
      bytestring[] = {0x41, 0x33, 0x54, 0x80, 0xFF, 0x99, 0x01, 0x78,
                      0x55, 0x20, 0xFE, 0xEE, 0x00, 0x00, 0x00, 0x00}, 
      bytestring_a[] = {0x44, 0x00, 0x00, 0x00, 0x01},
      bytestring_b[] = {0x30, 0xFF, 0xFF, 0x00, 0x00},
      bytestring_c[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
      bytestring_d[] = {0x00, 0x00, 0x00, 0x00, 0x01},
      bytestring_r[size];
   unsigned short int *short_int;
   unsigned int *just_int;
   unsigned long int *long_int;

   printf("\n");

   /* Part 1 : Data Types and Sizes */
   printf("Part 1: Data Types and Their Sizes\n");
   lineBreak(0, 0);
   printf("Byte Size: %u\n",      sizeof(unsigned char));
   printf("Short Int Size: %u\n", sizeof(short int));
   printf("Integer Size: %u\n",      sizeof(int));
   printf("Long Integer Size: %u\n",    sizeof(long int));
   lineBreak(0, 1);

   /* Part 2 : Byte Ordering */
   short_int = bytestring;
   just_int = bytestring;
   long_int = bytestring;

   printf("Part 2: Byte Ordering\n");
   lineBreak(0, 0);
   
   // No casting
   printf("Byte values of entire array in hex:\n");
   for (i = 0; i < 16; i++)
      printf("%02X ", bytestring[i]);
   printf("\n\n");

   // Cast as "short int"
   printf("Short Int versions of the first four values of an array in hex:\n");
   for (i = 0; i < 4; i++)
      printf("%04X ", short_int[i]);
   printf("\n\n");

   // Cast as "int"
   printf("Int versions of the first two values of an array in hex:\n");
   for (i = 0; i < 2; i++)
      printf("%08X ", just_int[i]);
   printf("\n\n"); 

   // Cast as "long int"
   printf("Long Int version of the first value of an array in hex:\n");
   printf("%016lX ", long_int[0]);
   printf("\n\n");
   printf("2e: ********\n");
   printf("2f: ********\n");
   printf("2g: ********\n");
   printf("2h: ********\n");
   lineBreak(0, 1);

   /* Part 3 : Unsigned Addition of Small Fields */
   printf("Part 3: Unsigned Addition of Small Fields\n");
   lineBreak(0, 0);
   printf("0x20 + 0x35 = 0x%02X\n", addUnsignedChar(0x20, 0x35, 0x00));
   printf("0x80 + 0x7F = 0x%02X\n", addUnsignedChar(0x80, 0x7F, 0x00));
   printf("0x80 + 0xFF = 0x%02X\n", addUnsignedChar(0x80, 0xFF, 0x00));
   printf("0xFF + 0x01 = 0x%02X\n", addUnsignedChar(0xFF, 0x01, 0x00));
   lineBreak(0, 1);

   /* Part 4 : Detecting a Carry Condition */
   printf("Part 4: Detecting a Carry Condition\n");
   lineBreak(0, 0);
   printf("0x20 + 0x35 Carry = %u\n", checkCarry(0x20, 0x35, 0x00));
   printf("0x80 + 0x7F Carry = %u\n", checkCarry(0x80, 0x7F, 0x00));
   printf("0x80 + 0xFF Carry = %u\n", checkCarry(0x80, 0xFF, 0x00));
   printf("0xFF + 0x01 Carry = %u\n", checkCarry(0xFF, 0x01, 0x00));
   lineBreak(0, 1);

   /* Part 5 : Signed Addition and Overflow */
   printf("Part 5: Signed Addition and Overflow\n");
   lineBreak(0, 0);
   printf("0x20 + 0x35 Overflow = %u\n", checkOverflow(0x20, 0x35));
   printf("0x80 + 0x7F Overflow = %u\n", checkOverflow(0x80, 0x7F));
   printf("0x80 + 0xFF Overflow = %u\n", checkOverflow(0x80, 0xFF));
   printf("0xFF + 0x01 Overflow = %u\n", checkOverflow(0xFF, 0x01));
   lineBreak(0, 1); 

   /* Part 6 : Extended Arithmetic */
   printf("Performing Extended Field Arithmetic\n");
   lineBreak(0, 0);
   printf("6a)\n   ");
   for (i = 0; i < size; i++)
      printf("0x%02X ", bytestring_a[i]);
   printf("\n+  ");
   for (i = 0; i < size; i++)
      printf("0x%02X ", bytestring_b[i]);
   printf("\n\n=  ");
   bytestring_r_carry = arbitraryByteAdd(bytestring_r, bytestring_a, bytestring_b, size, 0x00);
   for (i = 0; i < size; i++)
      printf("0x%02X ", bytestring_r[i]);
   printf("\n\nCarry Out = %u\n", bytestring_r_carry);
   printf("\n");

   printf("6b)\n   ");
   for (i = 0; i < size; i++)
      printf("0x%02X ", bytestring_c[i]);
   printf("\n+  ");
   for (i = 0; i < size; i++)
      printf("0x%02X ", bytestring_d[i]);
   printf("\n\n=  ");
   bytestring_r_carry = arbitraryByteAdd(bytestring_r, bytestring_c, bytestring_d, size, 0x00);
   for (i = 0; i < size; i++)
      printf("0x%02X ", bytestring_r[i]);
   printf("\n\nCarry Out = %u\n", bytestring_r_carry);
 
   lineBreak(0, 0);
   

   return 0;

   //TEST BEGIN
   printf("\nTEST\n");
   //TEST END

}
















