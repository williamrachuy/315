#include <stdio.h>

typedef struct {
   unsigned int sign;
   int exponent;
   int fraction;
} intFloat;

unsigned int convertFloat(float flt) {
   return (unsigned int)*(unsigned int *)&flt;
}

unsigned int uMultiply(unsigned int a, unsigned int b) {
   unsigned int i, product = 0;

   for (i = 0; i < 16; i++) {
      if (((a & 0x0000FFFF) >> i) & 0x01) product += (b & 0x0000FFFF) << i;
   }

   return product;
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

int main(void) {
   unsigned int fltAsInt;
   float fltIn, fltInA, fltInB, fltTemp;
   intFloat myFloatStruct = {.sign = 0, .exponent = 0, .fraction = 0};

   printf("Enter float: ");
   scanf("%g", &fltIn);
   fltAsInt = convertFloat(fltIn);
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

   return 0;
}