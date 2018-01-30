#include <stdio.h>
#define TRACE

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

unsigned int convFltTo754(float flt) {
   return (unsigned int)*(unsigned int *)&flt;
}

float conv754toFlt(unsigned int flt754) {
   return (float)*(float *)&flt754;
}

unsigned int uMultiply_Int(unsigned int a, unsigned int b) {
   short int i;
   unsigned int multiplicand, carryDetect, carryAdd = 0, product = 0;

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

unsigned long uMultiply_Long(unsigned int a, unsigned int b) {
   short int i;
   unsigned long product, multiplicand, oproduct, carry = 0L;

   product = (unsigned long)a;
   multiplicand = (unsigned long)b << 32;
   for (i = 0; i < 32; i++) {
      if (product & 1) {
         oproduct = product;
         carry = 0L;
         product += multiplicand;
         if (product < oproduct)
            carry = 0x8000000000000000L;
      } 	
      product = (product >> 1) | carry;
   }

   return product;
}

long int sMultiply(long int a, long int b){
   unsigned long product = 0, carryCheck, carryVal, multiplicand;
   short int i;
   long int result;
   unsigned short sign;

   sign = (unsigned short)((0x00000001 & (a >> 31)) ^ (0x00000001 & (b >> 31)));
   if(a < 0) a = -a;
   if(b < 0) b = -b;
   product = (unsigned long int)a & 0x7FFFFFFFFFFFFFFF;
   multiplicand = ((unsigned long int)b & 0x7FFFFFFFFFFFFFFF) << 32;
   for(i = 0; i < 32; i++){
      if (product & 0x0000000000000001) {
         carryCheck = product;
         carryVal = 0;
         product += multiplicand;
         if(product < carryCheck){
            carryVal = 0x8000000000000000;
         }
      }
<<<<<<< HEAD
      product = (product >> 1) | carryVal;
=======
      
      product = (product >> 1) | carryVal;
      
      printf("Current iteration %d: 0x%016lx\n", i, product);
>>>>>>> 321a240a29d659ff2934edf6ebf1b4fba6445e60
   }
   result = (long int)product;
   if (sign) result *= -1;

   return result;
}

intFloat *extFloat(intFloat *fltStruct, float flt) {
<<<<<<< HEAD
   unsigned int fltConv = convFltTo754(flt);
=======
   unsigned int fltConv = (unsigned int)*(unsigned int *)&flt;
>>>>>>> 321a240a29d659ff2934edf6ebf1b4fba6445e60

   if (flt == 0) {
      fltStruct->sign     = 0;
      fltStruct->exponent = 0;
      fltStruct->fraction = 0;
   } else {
      fltStruct->sign     = fltConv & 0x80000000;
      fltStruct->exponent = ((fltConv >> 23) & 0x000000FF) - 127;
      fltStruct->fraction = ((fltConv << 7) & 0x3FFFFF80) | 0x40000000;
<<<<<<< HEAD
      if (fltStruct->sign == 0x80000000)
         fltStruct->fraction *= -1;
=======
      
      #ifdef TRACE
      printf("extFloat before negation: fltStruct.fraction = 0x%08x\n", fltStruct->fraction);
      
      #endif
      
      if(fltStruct->sign) fltStruct->fraction *= -1;
      
      #ifdef TRACE
      printf("extFloat: float = %g, fltStruct.sign = 0x%08x, fltStruct.exponent = 0x%08x, fltStruct.fraction = 0x%08x\n",
         flt, fltStruct->sign, fltStruct->exponent, fltStruct->fraction);
      
      #endif
>>>>>>> 321a240a29d659ff2934edf6ebf1b4fba6445e60
   }

   return fltStruct;
}

float packFloat(intFloat *fltStruct) {
   unsigned int fltConv = 0;
<<<<<<< HEAD

   fltConv |= fltStruct->sign & 0x80000000;
   fltConv |= ((fltStruct->exponent + 127) << 23) & 0x7F800000;
   if (fltStruct->sign & 0x80000000)
      fltStruct->fraction *= -1;
   fltConv |= (fltStruct->fraction >> 7) & 0x007FFFFF;

   return conv754toFlt(fltConv);
=======
   int fracSigned;

   fltConv |= ((fltStruct->exponent + 127) << 23) & 0x7F800000;
   fltConv |= fltStruct->sign & 0x80000000;
   fracSigned = fltStruct->fraction;
   if(fltStruct->sign) fracSigned *= -1;
   fltConv |= ((fracSigned & ~0x40000000) >> 7) & 0x007FFFFF;

   return (float)*(float *)&fltConv;
>>>>>>> 321a240a29d659ff2934edf6ebf1b4fba6445e60
}

intFloat *scaleFloat(intFloat *fltStruct, int n) {
   fltStruct->fraction >>= n;
   fltStruct->exponent += n;

   return fltStruct;
}

intFloat *normalizeFloat(intFloat *fltStruct) {
<<<<<<< HEAD
   if (fltStruct->fraction == 0) return fltStruct;
   while (!((fltStruct->fraction ^ (fltStruct->fraction >> 1)) & 0x40000000)) {
=======
   unsigned int signFlag = (fltStruct->fraction >> 1);

   if (fltStruct->fraction == 0){
      fltStruct->exponent = 0;
      return fltStruct;
   }

#ifdef TRACE   
   printf("Fraction before normalize: 0x%08x", fltStruct->fraction);
#endif

   // Probably need to do different operations depending on whether 
   // the sign is positive or negative - original solution was for unsigned!
   //
   // while the sign bit and bit next to sign bit are the same, shift left
   // XOR the two bits, if the result is a 0 that means the two bits are the same and 
   // we need to keep shifting. If the XOR produces a 1 that means the two bits are
   // different and we have finished normalizing
   while (!(((fltStruct->fraction & 0x80000000) ^ ((fltStruct->fraction) << 1) & 0x80000000)))
   {
>>>>>>> 321a240a29d659ff2934edf6ebf1b4fba6445e60
      fltStruct->fraction <<= 1;
      fltStruct->exponent--;
   }

<<<<<<< HEAD
=======
#ifdef TRACE
   printf("Fraction during normalize: 0x%08x\n", fltStruct->fraction);
#endif
   
>>>>>>> 321a240a29d659ff2934edf6ebf1b4fba6445e60
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
   
#ifdef TRACE
   printf("Exponents before add: A = 0x%08x, B = 0x%08x\n", fltStructA.exponent, fltStructB.exponent);
   printf("Fractions before add: A = 0x%08x, B = 0x%08x\n", fltStructA.fraction, fltStructB.fraction);
#endif
   
   exponentDiff = fltStructA.exponent - fltStructB.exponent;
   if (exponentDiff > 0) scaleFloat(&fltStructB, exponentDiff);
   if (exponentDiff < 0) scaleFloat(&fltStructA, -exponentDiff);
   fltStructR.fraction =
         (fltStructA.fraction) + (fltStructB.fraction);
         
   fltStructR.sign |= fltStructR.fraction & 0x80000000;
   fltStructR.exponent = fltStructA.exponent;
   
#ifdef TRACE
   printf("Exponents after add before normalize: R = 0x%08x\n", fltStructR.exponent);
   printf("Fractions after add before normalize: R = 0x%08x\n", fltStructR.fraction);
#endif   
   
   normalizeFloat(&fltStructR);

<<<<<<< HEAD
=======
#ifdef TRACE
   printf("Exponents after add after normalize: R = 0x%08x\n", fltStructR.exponent);
   printf("Fractions after add after normalize: R = 0x%08x\n", fltStructR.fraction);
#endif   

   
>>>>>>> 321a240a29d659ff2934edf6ebf1b4fba6445e60
   return packFloat(&fltStructR);
}

float subFloat(float a, float b){
   intFloat fltStructA, fltStructB, fltStructR;
   int exponentDiff;
   
   fltStructR.sign     = 0;
   fltStructR.exponent = 0;
   fltStructR.fraction = 0;
   extFloat(&fltStructA, a);
   extFloat(&fltStructB, b);
   
#ifdef TRACE
   printf("Exponents before sub: A = 0x%08x, B = 0x%08x\n", fltStructA.exponent, fltStructB.exponent);
   printf("Fractions before sub: A = 0x%08x, B = 0x%08x\n", fltStructA.fraction, fltStructB.fraction);
#endif
   
   exponentDiff = fltStructA.exponent - fltStructB.exponent;
   if (exponentDiff > 0) scaleFloat(&fltStructB, exponentDiff);
   if (exponentDiff < 0) scaleFloat(&fltStructA, -exponentDiff);
   fltStructR.fraction = 
      ((fltStructA.fraction >> 1) - (fltStructB.fraction >> 1));
   fltStructR.sign |= fltStructR.fraction & 0x80000000;
   fltStructR.exponent = fltStructA.exponent + 1;
<<<<<<< HEAD
   normalizeFloat(&fltStructR);
   
   return packFloat(&fltStructR);   
=======

#ifdef TRACE
   printf("Exponents after sub before normalize: A = 0x%08x, B = 0x%08x\n", fltStructR.exponent, fltStructR.exponent);
   printf("Fractions after sub before normalize: A = 0x%08x, B = 0x%08x\n", fltStructR.fraction, fltStructR.fraction);
#endif     

   normalizeFloat(&fltStructR);
>>>>>>> 321a240a29d659ff2934edf6ebf1b4fba6445e60
   
#ifdef TRACE
   printf("Exponents after sub after normalize: A = 0x%08x, B = 0x%08x\n", fltStructR.exponent, fltStructR.exponent);
   printf("Fractions after sub after normalize: A = 0x%08x, B = 0x%08x\n", fltStructR.fraction, fltStructR.fraction);
#endif     
   
   return packFloat(&fltStructR);   
}

float fMultiply_754(float a, float b){
   intFloat fltStructA, fltStructB, fltStructR;

   fltStructR.sign     = 0;
   fltStructR.exponent = 0;
   fltStructR.fraction = 0;
   extFloat(&fltStructA, a);
   extFloat(&fltStructB, b);
<<<<<<< HEAD
   fltStructR.sign = fltStructA.sign ^ fltStructB.sign; 
   fltStructR.exponent = fltStructA.exponent + fltStructB.exponent;
   fltStructR.fraction = (uMultiply_Long(fltStructA.fraction << 1, fltStructB.fraction << 1) >> 32) & 0xFFFFFFFF;
=======
   
   fltStructR.exponent = fltStructA.exponent + fltStructB.exponent;
   fltStructR.sign = fltStructA.sign ^ fltStructB.sign;
   
   multiplyReturn = umul2_long(fltStructA.fraction << 1, fltStructB.fraction << 1);
   
   fltStructR.fraction = (multiplyReturn >> 32) & 0xFFFFFFFF;
   
>>>>>>> 321a240a29d659ff2934edf6ebf1b4fba6445e60
   normalizeFloat(&fltStructR);

   return packFloat(&fltStructR);
}

void printPart1(char n, unsigned int a, unsigned int b) {
   printf("1%c. a=0x%04X, b=0x%04X c=0x%08X\n", n, a, b, uMultiply_Int(a, b));
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
   
   
   /*
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
   
   */

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
<<<<<<< HEAD

   float a, b;

   while (1) {
      printf("Enter two floats: ");
      scanf("%f %f", &a, &b);
      printf("%f + %f = %f\n\n", a, b, addFloat(a, b));
   }
=======
   float fltInA, fltInB, fltTemp;

   printf("Enter two floats: ");
   scanf("%g%g", &fltInA, &fltInB);
   printf("Addition: %g\n", addFloat(fltInA, fltInB));
   printf("Subtraction: %g\n", subtractFloat(fltInA, fltInB));
   printf("Multiplication: %g\n", fmul(fltInA, fltInB));
>>>>>>> 321a240a29d659ff2934edf6ebf1b4fba6445e60

   return 0;
}
