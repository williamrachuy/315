#include <stdio.h>

typedef struct {
   unsigned int sign;
   int exponent;
   int fraction;
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
   while (1) {
      float fltA, fltB;
      printf("Enter two floats: ");
      scanf("%f%f", &fltA, &fltB);
      printf("%f + %f = %f\n\n", fltA, fltB, addFloat(fltA, fltB));
   }
   #endif

   #ifdef SUBFLT
   while (1) {
      float fltA, fltB;
      printf("Enter two floats: ");
      scanf("%f%f", &fltA, &fltB);
      printf("%f - %f = %f\n\n", fltA, fltB, subFloat(fltA, fltB));
   }
   #endif

   return 0;
}