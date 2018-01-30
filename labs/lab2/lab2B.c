#include <stdio.h>

typedef struct {
   unsigned int sign;
   int exponent, fraction;
} intFloat;

unsigned int convFltTo754(float flt) {
   return (unsigned int)*(unsigned int *)&flt;
}

float conv754toFlt(unsigned int flt754) {
   return (float)*(float *)&flt754;
}

intFloat *normalizeFloat(intFloat *fltStruct) {
   if (fltStruct->fraction == 0) return fltStruct;
   if (fltStruct->sign & 0x80000000) fltStruct->fraction *= -1;
   while (!((fltStruct->fraction ^ (fltStruct->fraction >> 1)) & 0x40000000)) {
      fltStruct->fraction <<= 1;
      fltStruct->exponent--;
   }
   if (fltStruct->sign & 0x80000000) fltStruct->fraction *= -1;

   return fltStruct;
}

intFloat *scaleFloat(intFloat *fltStruct, int n) {
   fltStruct->fraction >>= n;
   fltStruct->exponent += n;

   return fltStruct;
}

intFloat *extFloat(intFloat *fltStruct, float flt) {
   unsigned int fltConv = convFltTo754(flt);

   if (flt == 0) {
      fltStruct->sign     = 0;
      fltStruct->exponent = 0;
      fltStruct->fraction = 0;
   } else {
      fltStruct->sign     = fltConv & 0x80000000;
      fltStruct->exponent = ((fltConv >> 23) & 0x000000FF) - 127;
      fltStruct->fraction = ((fltConv << 7) & 0x3FFFFF80) | 0x40000000;
      if (fltStruct->sign == 0x80000000)
         fltStruct->fraction *= -1;
   }

   return fltStruct;
}

float packFloat(intFloat *fltStruct) {
   unsigned int fltConv = 0;

   if (fltStruct->fraction != 0) {
      fltConv |= fltStruct->sign & 0x80000000;
      fltConv |= ((fltStruct->exponent + 127) << 23) & 0x7F800000;
      if (fltStruct->sign & 0x80000000)
         fltStruct->fraction *= -1;
      fltConv |= (fltStruct->fraction >> 7) & 0x007FFFFF;
   }

   return conv754toFlt(fltConv);
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
   if (exponentDiff > 0) scaleFloat(&fltStructB,  exponentDiff);
   if (exponentDiff < 0) scaleFloat(&fltStructA, -exponentDiff);
   fltStructR.fraction = (fltStructA.fraction >> 1) + (fltStructB.fraction >> 1);
   fltStructR.exponent = fltStructA.exponent + 1;
   fltStructR.sign = (unsigned int)fltStructR.fraction & 0x80000000;
   normalizeFloat(&fltStructR);

   return packFloat(&fltStructR);
}

float subFloat(float a, float b) {
   intFloat fltStructA, fltStructB, fltStructR;
   int exponentDiff;

   fltStructR.sign     = 0;
   fltStructR.exponent = 0;
   fltStructR.fraction = 0;
   extFloat(&fltStructA, a);
   extFloat(&fltStructB, b);
   exponentDiff = fltStructA.exponent - fltStructB.exponent;
   if (exponentDiff > 0) scaleFloat(&fltStructB,  exponentDiff);
   if (exponentDiff < 0) scaleFloat(&fltStructA, -exponentDiff);
   fltStructR.fraction = fltStructA.fraction - fltStructB.fraction;
   fltStructR.exponent = fltStructA.exponent;
   fltStructR.sign = (unsigned int)fltStructR.fraction & 0x80000000;
   normalizeFloat(&fltStructR);

   return packFloat(&fltStructR);
}

unsigned int uMultiply_Int(unsigned int a, unsigned int b) {
   unsigned int multiplicand, product, carryDetect, carryAdd = 0;
   short int i;

   product = (unsigned int)a;
   multiplicand = ((unsigned int) b) << 16;   
   for (i = 0; i < 16; i++) {
      if (product & 0x00000001) {
         carryDetect = product;
         carryAdd = 0;         
         product += multiplicand;      
         if (product < carryDetect) carryAdd = 0x80000000;
      }
      product = (product >> 1) | carryAdd;
   }

   return product;
}

unsigned long uMultiply_Long(unsigned int a, unsigned int b) {
   unsigned long multiplicand, product, carryDetect, carryAdd = 0L;
   short int i;

   product = (unsigned long)a;
   multiplicand = (unsigned long)b << 32;
   for (i = 0; i < 32; i++) {
      if (product & 1) {
         carryDetect = product;
         carryAdd = 0;
         product += multiplicand;
         if (product < carryDetect) carryAdd = 0x8000000000000000L;
      }
      product = (product >> 1) | carryAdd;
   }

   return product;
}

long int sMultiply_Long(long int a, long int b) {
   unsigned long multiplicand, product, carryDetect, carryAdd;
   short int i, sign = 1;

   if (a < 0) {
      a = -a;
      sign *= -1;
   }
   if (b < 0) {
      b = -b;
      sign *= -1;
   }
   product = (unsigned long)a & 0x7FFFFFFFFFFFFFFF;
   multiplicand = ((unsigned long)b & 0x7FFFFFFFFFFFFFFF) << 32;
   for (i = 0; i < 32; i++){
      if (product & 0x0000000000000001) {
         carryDetect = product;
         carryAdd = 0x0000000000000000;
         product += multiplicand;         
         if (product < carryDetect) carryAdd = 0x8000000000000000;
      }
      product = (product >> 1) | carryAdd;
   }
   printf("%u %u\n", a < 0, b < 0);
   if (sign < 0) return -(long int)product;

   return product;
}

float fMultiply(float a, float b){
   intFloat fltStructA, fltStructB, fltStructR;

   fltStructR.sign     = 0;
   fltStructR.exponent = 0;
   fltStructR.fraction = 0;
   extFloat(&fltStructA, a);
   extFloat(&fltStructB, b);
   fltStructR.sign = fltStructA.sign ^ fltStructB.sign;
   fltStructR.exponent = fltStructA.exponent + fltStructB.exponent;
   if (fltStructA.sign & 0x80000000) fltStructA.fraction *= -1;
   if (fltStructB.sign & 0x80000000) fltStructB.fraction *= -1;
   fltStructR.fraction =
      (uMultiply_Long(fltStructA.fraction << 1, fltStructB.fraction << 1) >> 32) & 0xFFFFFFFF;
   if (fltStructR.sign & 0x80000000) fltStructR.fraction *= -1;
   normalizeFloat(&fltStructR);

   return packFloat(&fltStructR);
}

/*******************************************************************************
 *
 *    Functions used for output
 *
 ******************************************************************************/

void printPart1(char n, unsigned int a, unsigned int b) {
   printf("1%c. a=0x%04X, b=0x%04X c=0x%08X\n", n, a, b, uMultiply_Int(a, b));
}

void printPart2(char n, unsigned int flt754) {
   float flt = conv754toFlt(flt754);
   intFloat fltStruct;

   extFloat(&fltStruct, flt);
   printf("2%c. Test case: 0x%08X\n", n, flt754);
   printf("    Float: %.6f\n", flt);
   printf("    Exponent: %d\n", fltStruct.exponent);
   printf("    Fraction: 0x%08X\n", fltStruct.fraction);
}

void printPart3(char n, unsigned int flt754) {
   float flt = conv754toFlt(flt754);
   intFloat fltStruct;

   extFloat(&fltStruct, flt);
   printf("3%c. Test case: 0x%08X\n", n, flt754);
   printf("    Float: %.6f\n", packFloat(&fltStruct));
}

void printPart4(char n, int exponent, int fraction) {
   intFloat fltStruct;

   fltStruct.sign = 0;
   if (fraction < 0) fltStruct.sign = 0x80000000;
   fltStruct.exponent = exponent;
   fltStruct.fraction = fraction;
   normalizeFloat(&fltStruct);
   printf("4%c. Exp = %d, Frac = 0x%08X\n", n, fltStruct.exponent, fltStruct.fraction);
}

void printPart5(char n, unsigned int a, unsigned int b) {
   float flt = addFloat(conv754toFlt(a), conv754toFlt(b));

   printf("5%c. 0x%08X and 0x%08X (%.6f + %.6f)\n", n, a, b, conv754toFlt(a), conv754toFlt(b));
   printf("    Sum: 0x%08X (%.6f)\n", convFltTo754(flt), flt);
}

void printPart6(char n, unsigned int a, unsigned int b) {
   float flt = subFloat(conv754toFlt(a), conv754toFlt(b));

   printf("6%c. 0x%08X and 0x%08X (%.6f - %.6f)\n", n, a, b, conv754toFlt(a), conv754toFlt(b));
   printf("    Diff: 0x%08X (%.6f)\n", convFltTo754(flt), flt);
}

void printPart7(char n, unsigned int a, unsigned int b) {
   float flt = fMultiply(conv754toFlt(a), conv754toFlt(b));

   printf("7%c. 0x%08X and 0x%08X (%.6f * %.6f)\n", n, a, b, conv754toFlt(a), conv754toFlt(b));
   printf("     Prod: 0x%08X (%.6f)\n", convFltTo754(flt), flt);

}

void linePart(unsigned int n) {
   printf("PART %u ========================================================\n", n);
}

void lineBreak(unsigned int n) {
   unsigned int i;

   printf("===============================================================\n");
   for (i = 0; i < n; i++)
      printf("\n");
}

int main(void) {
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
   printPart4('a', 0x00000001, 0x40000000);
   printPart4('b', 0x00000000, 0x00200000);
   printPart4('c', 0x0000000C, 0xFFC00000);
   printPart4('d', 0xFFFFFFF8, 0x02000000);
   lineBreak(1);

   linePart(5);
   printPart5('a', 0xBF800000, 0x3F800000);
   printPart5('b', 0x3F800000, 0x3E800000);
   printPart5('c', 0x40800000, 0xBE000000);
   lineBreak(1);

   linePart(6);
   printPart6('a', 0x40400000, 0x3F800000);
   printPart6('b', 0x40400000, 0xBF800000);
   printPart6('c', 0x40000000, 0x40000000);
   lineBreak(1);

   linePart(7);
   printPart7('a', 0x40200000, 0x40200000);
   printPart7('b', 0xC1700000, 0x45800000);
   lineBreak(1);
   
   return 0;
}