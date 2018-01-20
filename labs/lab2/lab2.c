#include <stdio.h>

unsigned int maskSign     = 0x80000000,
             maskExponent = 0x7F800000,
             maskMantissa = 0x007FFFFF,
             maskLower    = 0x0000FFFF,
             shftSign     = 31,
             shftExponent = 23;

typedef struct {
   unsigned int sign;
   int exponent;
   int mantissa;
} intFloat;

unsigned int fltConv32(float flt) {
   return (unsigned int)*(unsigned int *)&flt;
}

unsigned int uMultiply(unsigned int a, unsigned int b) {
   unsigned int i, product = 0;

   for (i = 0; i < 16; i++) {
      if (((a & maskLower) >> i) & 0x01)
         product += ((b & maskLower) << i);
   }

   return product;
}

/*
intFloat *extFloat(intFloat *fltStruct, float flt) {
   unsigned int fltConverted = (unsigned int)*(unsigned int *)&flt;

   fltStruct->sign     = (fltConverted & maskSign)     >> shftSign;
   fltStruct->exponent = (fltConverted & maskExponent) >> shftExponent;
   fltStruct->mantissa = (fltConverted & maskMantissa);

   return fltStruct;
}
*/

intFloat *extFloat(intFloat *fltStruct, float flt) {
   unsigned int fltConv = (unsigned int)*(unsigned int *)&flt;

   // add test for mantissa = 0?

   fltStruct->sign     = fltConv & 0x80000000;
   fltStruct->exponent = ((fltConv >> 23) & 0x000000FF) - 127;
   fltStruct->mantissa = ((fltConv << 7) & 0x3FFFFF80) | 0x40000000;

   if (fltStruct->sign)
      fltStruct->mantissa = -fltStruct->mantissa;

   return fltStruct;
}

/*
float packFloat(intFloat *fltStruct) {
   unsigned int flt = 0;

   flt = (fltStruct->sign << shftSign) |
         (fltStruct->exponent << shftExponent) |
         (fltStruct->mantissa);

   return (float)*(float *)&flt;
}
*/

float packFloat(intFloat *fltStruct) {
   unsigned int fltConv = 0;

   fltConv |= fltStruct->sign & 0x80000000;
   fltConv |= (fltStruct->exponent + 127) & 

   return (float)*(float *)&fltConv;
}
int main(void) {
   unsigned int fltAsInt;
   float fltIn, fltTemp;
   intFloat myFloatStruct = {.sign = 0, .exponent = 0, .mantissa = 0};

   printf("Enter float: ");
   scanf("%g", &fltIn);

   fltAsInt = fltConv32(fltIn);
   printf("Float as IEEE754: 0x%08X\n", fltAsInt);
   
   extFloat(&myFloatStruct, fltIn);

   printf("Sign: 0x%08X Exponent: 0x%08X Mantissa: 0x%08X\n",
         myFloatStruct.sign, myFloatStruct.exponent, myFloatStruct.mantissa);

   fltTemp = packFloat(&myFloatStruct);
   printf("Packed float: %g\n", fltTemp);

   return 0;
}
