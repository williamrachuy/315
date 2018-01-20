/*----------------------------------------------------------------------*
 *	umul2_long.c								*
 *	This uses "Method 2" - rightshifting the product.multiplier	*
 *									*
 *	Refer to *** marked code where it performs detection of carry,	*
 *	to allow computation of the full 32-bit result.			*
 *----------------------------------------------------------------------*/
 
#include "stdio.h"

/*----------------------------------------------------------------------*
 *	unsigned long int umul2_long(unsigned int a, unsigned int b);	*
 *									*
 *	unsigned multiply using Method 2, shifting the			*
 *	product.multiplier on each step. The Multiplicand stays the	*
 *	same.								*
 *									*
 *	This assumes an unsigned multiplier and an unsigned multiplicand*
 *	and produces a 64-bit result.					*
 *----------------------------------------------------------------------*/

unsigned long int umul2_long(unsigned int a, unsigned int b)
  {
  short int i;
  unsigned long int product;
  unsigned long int multiplicand;

  unsigned long int oproduct;	/* allowing us to detect carry */
  unsigned long int carry;		/* Will be set to 0x80000000 if carry */
  
  product = (unsigned long int) a;	/* Multiplier into low portion of product */

  multiplicand = ((unsigned long int) b ) << 32; /* Kept in high 32 bits */

  carry = 0L;				/* Initialize value in case there are no add's */

  for (i=0; i<32; i++)		/* Multiplying 32-bit fields */
    {

    if (product & 1)		/* Test low bit of multiplier */
      {
      oproduct = product;	/* *** Old value of product before addition */
      carry = 0L;		/* Initialize - NO carry */

/*----------------------------------------------------------------------*
 *	This is where we add the multiplicand to the high 16 bits of	*
 *	the product.   Note the product ends up being 32 bits long.	*
 *----------------------------------------------------------------------*/

      product += multiplicand;	/* Add to high 16 bits */
				/* (because multiplicand was up-shifted) */

      if (product < oproduct)	/* *** Check for carry after the last add */
         {
         carry = 0x8000000000000000L;	/* If carry, force shifted high bit to 1 */
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
 *	This uses umult2_long, assumes 32-bit multiplier,multiplicand,	*
 *	to produce a 64-bit product.					*
 *----------------------------------------------------------------------*/

int main()
  {
  unsigned long int product;
  unsigned int multiplier = 0xFFFFFFFF;
  unsigned int multiplicand = 0x12345678;
  
  product = umul2_long(multiplier, multiplicand);
  
  printf("            Product is: %016lX\n", product);


/* Now cross-check the product by doing a regular C multiply with * operator. */

  product = ( (unsigned long) multiplier * (multiplicand) ) ; /* result of regular multiply */
  printf("Cross-check Product is: %016lX\n", product);
  
  return(0);  

  }

