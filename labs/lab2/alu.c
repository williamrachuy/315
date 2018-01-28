#include <stdio.h>
#include "alu.h"

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
      
      product = (product >> 1);
   }
  
   result = (long int)product;
   if(sign) result *= -1;
   
   return result;
}

intFloat *extFloat(intFloat *fltStruct, float flt) {
   unsigned int fltConv = (unsigned int)*(unsigned int *)&flt;

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

   return fltStruct;
}

float packFloat(intFloat *fltStruct) {
   unsigned int fltConv = 0;

   fltConv |= fltStruct->sign & 0x80000000;
   fltConv |= ((fltStruct->exponent + 127) << 23) & 0x7F800000;
   if (fltStruct->sign) fltStruct->fraction = -fltStruct->fraction;
   fltConv |= ((fltStruct->fraction & ~0x40000000) >> 7) & 0x007FFFFF;

   return (float)*(float *)&fltConv;
}

intFloat *scaleFloat(intFloat *fltStruct, int n) {
   fltStruct->fraction >>= n;
   fltStruct->exponent += n;

   return fltStruct;
}

intFloat *normalizeFloat(intFloat *fltStruct) {
   unsigned int signFlag = (fltStruct->fraction >> 1);

   if (fltStruct->fraction == 0) return fltStruct;
   
   while (((fltStruct->fraction ^ signFlag) & 0x40000000) == 0) {
      fltStruct->fraction <<= 1;
      fltStruct->exponent--;
   }

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

   
   fltStructR.exponent = fltStructA.exponent + fltStructB.exponent;

   multiplyReturn = sMultiply(fltStructA.fraction, fltStructB.fraction);
   
   if(multiplyReturn < 0){
      fltStructR.sign = 0x80000000;
      multiplyReturn *= -1;
   }
   
   fltStructR.fraction = (int)(multiplyReturn >> 32);
   
   normalizeFloat(&fltStructR);

   return packFloat(&fltStructR);
}
