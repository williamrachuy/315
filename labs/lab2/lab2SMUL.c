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

int main(void) {
   #ifdef EXTRACT
   float flt;
   intFloat fltStruct;
   while (1) {
      printf("Enter float: ");
      scanf("%f", &flt);
      extFloat(&fltStruct, flt);
      printf("Extracted: %f, Sign: %u, Exponent: %d, Fraction: %d\n", flt,
         fltStruct.sign, fltStruct.exponent, fltStruct.fraction);
      printf("Packed: %f\n\n", packFloat(&fltStruct));
   }
   #endif

   #ifdef ADDFLT
   float fltA, fltB;
   while (1) {
      printf("Enter two floats: ");
      scanf("%f%f", &fltA, &fltB);
      printf("%f + %f = %f\n\n", fltA, fltB, addFloat(fltA, fltB));
   }
   #endif

   #ifdef SUBFLT
   float fltA, fltB;
   while (1) {
      printf("Enter two floats: ");
      scanf("%f%f", &fltA, &fltB);
      printf("%f - %f = %f\n\n", fltA, fltB, subFloat(fltA, fltB));
   }
   #endif

   #ifdef UMUL_INT
   unsigned int a, b;
   while (1) {
      printf("Enter two ints: ");
      scanf("%u%i", &a, &b);
      printf("%u * %u = %u\n", a, b, uMultiply_Int(a, b));
   }
   #endif

   #ifdef UMUL_LONG
   unsigned int a, b;
   while (1) {
      printf("Enter two longs: ");
      scanf("%u%i", &a, &b);
      printf("%u * %u = %lu\n", a, b, uMultiply_Long(a, b));
   }
   #endif

   #ifdef SMUL_LONG
   int a, b;
   while (1) {
      printf("Enter two ints: ");
      scanf("%d%d", &a, &b);
      printf("%d * %d = %ld\n", a, b, sMultiply_Long(a, b));
   }
   #endif

   return 0;
}