/*----------------------------------------------------------------------*
 *	umul2.c								*
 *	This uses "Method 2" - rightshifting the product.multiplier	*
 *									*
 *	Refer to *** marked code where it performs detection of carry,	*
 *	to allow computation of the full 32-bit result.			*
 *----------------------------------------------------------------------*/
 
#include "stdio.h"

/*----------------------------------------------------------------------*
 *	unsigned int umul2(unsigned short int a, unsigned short int b);	*
 *									*
 *	unsigned multiply using Method 2, shifting the			*
 *	product.multiplier on each step. The Multiplicand stays the	*
 *	same.								*
 *									*
 *	This assumes a short int multiplier and a short int multiplicand*
 *	and produces a 32-bit result. It uses only 32-bit registers.	*
 *----------------------------------------------------------------------*/

unsigned int umul2(unsigned short int a, unsigned short int b)
  {
  unsigned int product;
  short int i;
  unsigned int multiplicand;

  unsigned int oproduct;	/* allowing us to detect carry */
  unsigned int carry;		/* Will be set to 0x80000000 if carry */
  
  product = (unsigned int) a;	/* Multiplier into low portion of product */
  multiplicand = ((unsigned int) b ) << 16; /* Kept in high 16 bits */

  carry = 0;			/* Initialize carry */
  for (i=0; i<16; i++)		/* Multiplying 16-bit fields */
    {
    if (product & 1)		/* Test low bit of multiplier */
      {
      oproduct = product;	/* *** Old value of product before addition */
      carry = 0;		/* Initialize - if NO carry */

/*----------------------------------------------------------------------*
 *	This is where we add the multiplicand to the high 16 bits of	*
 *	the product.   Note the product ends up being 32 bits long.	*
 *----------------------------------------------------------------------*/

      product += multiplicand;	/* Add to high 16 bits */
				/* (because multiplicand was up-shifted) */

      if (product < oproduct)	/* *** Check for carry after the last add */
         {
         carry = 0x80000000;	/* If carry, force shifted high bit to 1 */
         }
      }

/*----------------------------------------------------------------------*
 *	This is where we shift right the 32-bit product	and then "or"	*
 *	in the high bit based on the carry status.			*
 *----------------------------------------------------------------------*/
 	
    product = (product >> 1) | carry;	/* Shift right, shift in carry from left */
    
    }

  return (product);		/* Return 32-bit unsigned int */
  }
  


/*----------------------------------------------------------------------*
 *	main								*
 *	This uses umult2, assuming 16-bit multiplier,multiplicand, to	*
 *	product a 32-bit product.					*
 *----------------------------------------------------------------------*/

int main()
  {
  unsigned int product;
  short unsigned int multiplier = 0xFFFF;
  short unsigned int multiplicand = 0xFFFF;
  
  product = umul2(multiplier, multiplicand);
  
  printf("            Product is: %08X\n", product);


/* Now cross-check the product by doing a regular C multiply with * operator. */

  product = ( (unsigned) multiplier * (multiplicand) ) ; /* result of regular multiply */
  printf("Cross-check Product is: %08X\n", product);
  
  return(0);  

  }

