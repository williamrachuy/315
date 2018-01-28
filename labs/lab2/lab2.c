/*******************************************************************************
 * 
 *    THIS CODE IS STILL BEING DEVELOPED
 *
 *    OUR LAB TEAM IS TAKING ADVANTAGE OF THE EXTENSION THAT WAS GIVEN TO
 *    THE CLASS TODAY. PLEASE EMAIL ME AT wrachuy@calpoly.edu IF THERE IS
 *    ANY ISSUE WITH THIS SUBMISSION. THE LAB REPORT WILL BE SUBMITTED
 *    BEFORE THE EXTENDED DUE DATE.
 *
 *    Coded by Cameron Simpson and William Rachuy
 *
 ******************************************************************************/ 

#include <stdio.h>

typedef struct {
   unsigned int sign;
   int exponent;
   int fraction;
} intFloat;

void linePart(unsigned int n) {
   printf("PART %u ========================================================\n", n);
}

void lineBreak(unsigned int n) {
   unsigned int i;

   printf("===============================================================\n");
   for (i = 0; i < n; i++)
      printf("\n");
}

int decToBinInt(int);
int decToBinLong(long);

int decToBinInt(int dec) {
   if (dec == 0)
      return 0;
   else
      return (dec % 2 + 10 * decToBinInt(dec / 2));
}

int decToBinLong(long dec) {
   if (dec == 0)
      return 0;
   else
      return (dec % 2 + 10 * decToBinInt(dec / 2));
}

unsigned int convFltTo754(float flt) {
   return (unsigned int)*(unsigned int *)&flt;
}

float conv754toFlt(unsigned int flt754) {
   return (float)*(float *)&flt754;
}

unsigned int uMultiply(unsigned int a, unsigned int b) {
   unsigned int product = 0;
   short int i;
   unsigned int multiplicand;
   
   unsigned int carryDetect;
   unsigned int carryAdd = 0;

   product = (unsigned int) a;
   multiplicand = ((unsigned int) b) << 16;
   
   
   for (i = 0; i < 16; i++) {
      if(product & 0x00000001){
         carryDetect = product;
         carryAdd = 0;
         
         product += multiplicand;
      
         if(product < carryDetect){
            carryAdd = 0x80000000;
         }
      }
      
      product = (product >> 1) | carryAdd;
      
   }

   return product;
}

long int sMultiply(int a, int b){
   unsigned long int product = 0;
   short int i;
   unsigned long int multiplicand;
   long int result;

   // Store the resultant sign
   unsigned short int sign =
      (unsigned short)((0x00000001 & (a >> 31)) ^ (0x00000001 & (b >> 31)));
   //unsigned long int carryDetect;
   //unsigned long int carryAdd;
   
   // conv to ones compliment
   if(a < 0) a = -a;
   if(b < 0) b = -b;
   
   // Set product and multiplicand without sign bit
   product = (unsigned long)a & 0x7FFFFFFFFFFFFFFF;
   multiplicand = ((unsigned long)b & 0x7FFFFFFFFFFFFFFF) << 32;
   
   for(i = 0; i < 32; i++){
      if(product & 0x0000000000000001){
         product += multiplicand;
         
         //if(product & 0x8000000000000000)
      }
      
         product = product >> 1;
   }
   
#ifdef TRACE
   printf("Signed long int multiplication, a = 0x%x, b = 0x%x\n", a, b);
   printf("result = 0x%032lx\n\n", product);
#endif
   
   result = (long int)product;
   
   if(sign) result *= -1;
   
   return result;
}

intFloat *extFloat(intFloat *fltStruct, float flt) {
   unsigned int fltConv = (unsigned int)*(unsigned int *)&flt;

#ifdef TRACE
   printf("(extFloat) fltConv: 0x%08X\n", fltConv);
#endif

   if (flt == 0) {
      fltStruct->sign     = 0;
      fltStruct->exponent = 0;
      fltStruct->fraction = 0;
   } else {
      fltStruct->sign     = fltConv & 0x80000000;
      fltStruct->exponent = ((fltConv >> 23) & 0x000000FF) - 127;
      fltStruct->fraction = ((fltConv << 7) & 0x3FFFFF80) | 0x40000000;
      if (fltStruct->sign == 0x80000000)
         fltStruct->fraction = -fltStruct->fraction;
   }

#ifdef TRACE
   printf("(extFloat) sign: 0x%08X, exp: 0x%08X, frac: 0x%08X\n",
         fltStruct->sign, fltStruct->exponent, fltStruct->fraction);
#endif

   return fltStruct;
}

float packFloat(intFloat *fltStruct) {
   unsigned int fltConv = 0;

   fltConv |= fltStruct->sign & 0x80000000;
   fltConv |= ((fltStruct->exponent + 127) << 23) & 0x7F800000;
   if (fltStruct->sign) fltStruct->fraction = -fltStruct->fraction;
   fltConv |= ((fltStruct->fraction & ~0x40000000) >> 7) & 0x007FFFFF;

#ifdef TRACE
   printf("(packFloat) sign: 0x%08X, exp 0x%08X, frac: 0x%08X\n",
         fltStruct->sign, fltStruct->exponent, fltStruct->fraction);
   printf("(packFloat) fltConv: %g\n", (float)*(float *)&fltConv);
#endif

   return (float)*(float *)&fltConv;
}

intFloat *scaleFloat(intFloat *fltStruct, int n) {
   fltStruct->fraction >>= n;
   fltStruct->exponent += n;

#ifdef TRACE
   printf("(scaleFloat) sign: 0x%08X, exp: 0x%08X, frac: 0x%08X, n: %d\n",
         fltStruct->sign, fltStruct->exponent, fltStruct->fraction, n);
#endif

   return fltStruct;
}

intFloat *normalizeFloat(intFloat *fltStruct) {
   unsigned int signFlag = (fltStruct->fraction >> 1);

   if (fltStruct->fraction == 0) return fltStruct;
   
   // Probably need to do different operations depending on whether 
   // the sign is positive or negative - original solution was for unsigned!
   //
   // while the sign bit and bit next to sign bit are the same, shift left
   // XOR the two bits, if the result is a 0 that means the two bits are the same and 
   // we need to keep shifting. If the XOR produces a 1 that means the two bits are
   // different and we have finished normalizing
   // while (!(((fltStruct->fraction & 0x80000000) ^ ((floatStruct->fraction) << 1) & 0x80000000)))
   while (((fltStruct->fraction ^ signFlag) & 0x40000000) == 0) {
      fltStruct->fraction <<= 1;
      fltStruct->exponent--;
   }

#ifdef TRACE
   printf("(normalizeFloat) sign: 0x%08X, exp: 0x%08X, frac: 0x%08X\n",
         fltStruct->sign, fltStruct->exponent, fltStruct->fraction);
#endif

   return fltStruct;
}

float addFloat(float a, float b) {
   intFloat fltStructA, fltStructB, fltStructR;
   int exponentDiff;

   fltStructR.sign     = 0;
   fltStructR.exponent = 0;
   fltStructR.fraction = 0;
   extFloat(&fltStructA, a);
   extFloat(&fltStructB, b);
   exponentDiff = fltStructA.exponent - fltStructB.exponent;
   if (exponentDiff > 0) scaleFloat(&fltStructB, exponentDiff);
   if (exponentDiff < 0) scaleFloat(&fltStructA, -exponentDiff);
   fltStructR.fraction =
         (fltStructA.fraction >> 1) + (fltStructB.fraction >> 1);
   fltStructR.sign |= fltStructR.fraction & 0x80000000;
   fltStructR.exponent = fltStructA.exponent + 1;
   normalizeFloat(&fltStructR);

#ifdef TRACE
   printf("Post-normalize, addFloat called with a = %.8f, b = %.8f\n", a, b);
   printf("result: fraction = 0x%08X, exponent = 0x%08X (%d)\n\n",
         fltStructR.fraction, fltStructR.exponent, fltStructR.exponent);
#endif

   return packFloat(&fltStructR);
}

float subtractFloat(float a, float b){
   intFloat fltStructA, fltStructB, fltStructR;
   int exponentDiff;
   
   fltStructR.sign     = 0;
   fltStructR.exponent = 0;
   fltStructR.fraction = 0;
   extFloat(&fltStructA, a);
   extFloat(&fltStructB, b);
   exponentDiff = fltStructA.exponent - fltStructB.exponent;
   if (exponentDiff > 0) scaleFloat(&fltStructB, exponentDiff);
   if (exponentDiff < 0) scaleFloat(&fltStructA, -exponentDiff);
   
   fltStructR.fraction = 
      ((fltStructA.fraction >> 1) - (fltStructB.fraction >> 1));
      
   fltStructR.sign |= fltStructR.fraction & 0x80000000;
   fltStructR.exponent = fltStructA.exponent + 1;
   normalizeFloat(&fltStructR);
   

#ifdef TRACE
   printf("Post-normalize, addFloat called with a = %.8f, b = %.8f\n", a, b);
   printf("result: fraction = 0x%08X, exponent = 0x%08X (%d)\n\n",
         fltStructR.fraction, fltStructR.exponent, fltStructR.exponent);
#endif

   return packFloat(&fltStructR);   
   
}

float fmul(float a, float b){
   intFloat fltStructA, fltStructB, fltStructR;
   long int multiplyReturn;

   fltStructR.sign     = 0;
   fltStructR.exponent = 0;
   fltStructR.fraction = 0;
   extFloat(&fltStructA, a);
   extFloat(&fltStructB, b);
  
   fltStructR.exponent = fltStructA.exponent + fltStructB.exponent + 1;

   printf("0b%d\n", decToBinInt(fltStructA.fraction));
   multiplyReturn = sMultiply(fltStructA.fraction, fltStructB.fraction);
   printf("0b%l\n", decToBinLong(multiplyReturn));
   
   if(fltStructA.sign ^ fltStructB.sign){
      fltStructR.sign = 0x80000000;
   }
   
   fltStructR.fraction = (int)(multiplyReturn >> 32);
   printf("0b%d\n", decToBinInt(fltStructR.fraction));
   
   normalizeFloat(&fltStructR);

#ifdef TRACE
   printf("Post-normalize, addFloat called with a = %.8f, b = %.8f\n", a, b);
   printf("result: fraction = 0x%08X, exponent = 0x%08X (%d)\n\n",
         fltStructR.fraction, fltStructR.exponent, fltStructR.exponent);
#endif

   return packFloat(&fltStructR);
}

void printPart1(char n, unsigned int a, unsigned int b) {
   printf("1%c. a=0x%04X, b=0x%04X c=0x%08X\n", n, a, b, uMultiply(a, b));
}

void printPart2(char n, unsigned int flt754) {
   float flt = conv754toFlt(flt754);
   intFloat fltStruct;

   extFloat(&fltStruct, flt);
   printf("2%c. Test case: 0x%08X\n", n, flt754);
   printf("  Float: %.6f\n", flt);
   printf("  Exponent: %d\n", fltStruct.exponent);
   printf("  Fraction: 0x%08X\n", fltStruct.fraction);
}

void printPart3(char n, unsigned int flt754) {
   float flt = conv754toFlt(flt754), fltRet;
   intFloat fltStruct;

   extFloat(&fltStruct, flt);
   fltRet = packFloat(&fltStruct);
   printf("3%c. Test case: 0x%08X\n", n, flt754);
   printf("  Float: %.6f\n", fltRet);
}

void printPart4();

void printPart5();

void printPart6();

void printPart7();

int main(void) {
/*   unsigned int fltAsInt;
   float fltIn, fltInA, fltInB, fltTemp;
   int a = -888, b = 100;
   long int mulRes;
   intFloat myFloatStruct = {.sign = 0, .exponent = 0, .fraction = 0};

   unsigned int a, b;*/
   float a, b;

   printf("\n");

   linePart(1);
   printPart1('a', 0x0001, 0x0001);
   printPart1('b', 0x0001, 0xFFFF);
   printPart1('c', 0x8000, 0x0001);
   printPart1('d', 0x4000, 0x4000);
   printPart1('e', 0x8000, 0x8000);
   lineBreak(1);

   linePart(2);
   printPart2('a', 0x40C80000);
   printPart2('b', 0xC3000000);
   printPart2('c', 0x3E000000);
   printPart2('d', 0x3EAAAAAB);
   lineBreak(1);

   linePart(3);
   printPart3('a', 0x40C80000);
   printPart3('b', 0xC3000000);
   printPart3('c', 0x3E000000);
   printPart3('d', 0x3EAAAAAB);
   lineBreak(1);

   linePart(4);
   lineBreak(1);

   linePart(5);
   lineBreak(1);

   linePart(6);
   lineBreak(1);

   linePart(7);
   lineBreak(1);

/*   printf("Enter float: ");
   scanf("%g", &fltIn);
   fltAsInt = convFloat(fltIn);
   printf("Float as IEEE754: 0x%08X\n", fltAsInt);
   extFloat(&myFloatStruct, fltIn);
   printf("Sign: 0x%08X Exponent: 0x%08X fraction: 0x%08X\n",
         myFloatStruct.sign, myFloatStruct.exponent, myFloatStruct.fraction);
   fltTemp = packFloat(&myFloatStruct);
   printf("Packed float: %g\n", fltTemp);
   printf("\n");
   printf("Enter two floats: ");
   scanf("%g%g", &fltInA, &fltInB);
   fltTemp = addFloat(fltInA, fltInB);
   printf("Packed float: %g\n", fltTemp);
   
   printf("Enter two floats: ");
   scanf("%g%g", &fltInA, &fltInB);
   fltTemp = subtractFloat(fltInA, fltInB);
   printf("Packed float: %g\n", fltTemp);
   
   printf("Enter two floats: ");
   scanf("%g%g", &fltInA, &fltInB);
   fltTemp = fmul(fltInA, fltInB);
   printf("Packed float: %g\n", fltTemp);
   
   mulRes = sMultiply(a, b);
   printf("sMultiply: a = 0x%x, b = 0x%x, result = 0x%lx\n", a, b, mulRes); 
*/

   

   while (1) {
      scanf("%f%f", &a, &b);
      printf("%f\n", fmul(a, b));
   }

   return 0;
}