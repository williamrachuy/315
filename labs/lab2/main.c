#include <stdio.h>

unsigned int umultiply(unsigned int a, unsigned int b) {
   unsigned int product = 0, i;

   for (i = 0; i < 16; i++) {
      if ((a >> i) & 0x0001)
         product += (b << i);
         printf("%u, 0x%016X\n", i, product); 
   }

   return product;
}

unsigned int fltConv32(float g) {
   unsigned int k = (unsigned int) * (unsigned int *)&g;
   printf("g: %g, k: 0x%08X\n", g, k);
   return k;
}

int main(void) {
   unsigned int testInt;

   //fltConv32(3.14);
   //fltConv32(3.15);
   
   testInt = umultiply(0x0025, 0x0051);
   printf("0x%016X\n", testInt);   

   return 0;
}
