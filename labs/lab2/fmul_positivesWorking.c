float fmul(float a, float b){
   intFloat fltStructA, fltStructB, fltStructR;
   long int multiplyReturn;

   fltStructR.sign     = 0;
   fltStructR.exponent = 0;
   fltStructR.fraction = 0;
   extFloat(&fltStructA, a);
   extFloat(&fltStructB, b);
  
   fltStructR.exponent = fltStructA.exponent + fltStructB.exponent + 1;

   printf("0x%08X\n", fltStructA.fraction);
   multiplyReturn = sMultiply(fltStructA.fraction & 0x7FFFFFFF, fltStructB.fraction & 0x7FFFFFFF) << 1;
   printf("0x%08X\n", (unsigned int)(multiplyReturn >> 32));
   
   if(fltStructA.sign ^ fltStructB.sign){
      fltStructR.sign = 0x80000000;
   }
   
   fltStructR.fraction = (unsigned int)(multiplyReturn >> 32);
   printf("0x%08X\n", fltStructR.fraction);
   
   normalizeFloat(&fltStructR);

#ifdef TRACE
   printf("Post-normalize, addFloat called with a = %.8f, b = %.8f\n", a, b);
   printf("result: fraction = 0x%08X, exponent = 0x%08X (%d)\n\n",
         fltStructR.fraction, fltStructR.exponent, fltStructR.exponent);
#endif

   return packFloat(&fltStructR);
}
