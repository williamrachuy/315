/*----------------------------------------------------------------------*
 *	CPE 315 Lab 1 Examples and Hints				*
 *									*
 *	This is an example to help "jump-start" your floating point	*
 *	lab projects.  Your mileage may vary (i.e., it may be better!)	*
 *	This is only and example, and you can use this as a spring-	*
 *	board for your implementations.					*
 *									*
 *	NOTE: This does NOT implement the packing of the result, nor	*
 *	floating point subtract function.				*
 *									*
 *	Assumption in this implementation: it places the floating	*
 *	point number in a 2's complement number where the binary point	*
 *	is assumed to be AFTER the hidden '1'. That means, the first	*
 *	two bits of the 2's complement number are: sign, hidden '1'.	*
 *	Some may choose to scale it in a different way, and that's 	*
 *	great.								*
 *									*
 *	THIS IS JUST AN EXAMPLE IMPLEMENTATION.	It may or may not be	*
 *	the most efficient implementation, and ... there may, as always,*
 *	be gugs.  I mean bugs.   -dave						*
 *----------------------------------------------------------------------*/


#include <stdio.h>

typedef struct _intfloat
	{
	unsigned int sign;
	int exponent;
	int fraction;
	} INTFLOAT, *INTFLOAT_PTR;

/*----------------------------------------------------------------------*
 *	extract_float - extract floating point value as 2's complement	*
 *	fields. Resulting fraction has a binary point to the right	*
 *	of the hidden '1', i.e., S H [.] b-1 b-2 b-3 b-4 ... etc.	*
 *	Sign is initially 0, but will be changed when the value is	*
 *	negated (see: [F])						*
 *----------------------------------------------------------------------*/

void extract_float(INTFLOAT_PTR xp, float f)
	{
	unsigned floatbits;

	floatbits = * (unsigned int *) &f; // extract binary float #

// get the sign as a boolean, true if NEGATIVE   [A]

	xp->sign = (floatbits & 0x80000000);	// high-order bit = 1

// get the exponent from floatbits [B]

	xp->exponent = (floatbits >> 23) & 0x000000FF; // 8-bit exponent [C]
	xp->exponent -= 127;			// ... less the bias of 127 

// get the fraction as 2's complement, binary point is to the right of the hidden 1.

	xp->fraction = (floatbits << 7) & 0x3FFFFF80;	// 23 bits of fraction [D]  
	xp->fraction |= 0x40000000;			// force hidden '1' (0 1 [.] b-1 b-2 b-3 etc.) [E]
	if (xp->sign) xp->fraction = -xp->fraction;	// negate 2's comp if negative [F]

// Optionable Debug info, print out the result:
	
//	printf("Test case: %.8f, ", f);
//	if (xp->sign) printf("NEGATIVE, ");
//	printf ("Fraction is: 0x%08X, ", xp->fraction);
//	printf ("Exponent is: 0x%08X\n", xp->exponent);
//	printf("\n");

	}



/*----------------------------------------------------------------------*
 *	scale_it - takes an INTFLOAT argument and scales it down 'n'	*
 *	positions by shifting it right, adding n to the exponent.	*
 *									*
 *	Assumes pointer xp to the element to be scaled down, and n	*
 *	is the number of positions.					*
 *----------------------------------------------------------------------*/

	void scale_it(INTFLOAT_PTR xp, int n)
	  {
	  int i;

	  xp->fraction >>= n;	// shift right by number of positions to scale
	  xp->exponent += n;	// Increase exponent by amount scaled
	  }


/*----------------------------------------------------------------------*
 *	void normalize(INTFLOAT_PTR xp)					*
 *									*
 *	1. This just normalizes the value of the item pointed to by	*
 *	   xp. It does NOT return a float, since that will be done 	*
 *	   by the pack function.					*
 *	2. This will shift xp->fraction left UNTIL it the sign bit is	*
 *	   no longer equal to the adjacent bit.  It decrements the	*
 *	   exponent for every shift it makes.				*
 *									*
 *	This is just an example. YOUR IMPLEMENTATION MAY DIFFER !	*
 *----------------------------------------------------------------------*/

void normalize(INTFLOAT_PTR xp)
  {
  unsigned int signflag;

  signflag = (xp->fraction >> 1); // this is just the sign bit,
					//         moved over 1

  if (xp->fraction == 0) return;	// Escape clause: it's zero!

  while ( ((xp->fraction ^ signflag) & 0x40000000)==0) // As long as 0 (equal)
				// xor the sign with next bit, 0 means equal
    {
    xp->fraction <<= 1;			// Shift the fraction left
    xp->exponent--;			// Decrement the exponent
    }
  }


/*----------------------------------------------------------------------*
 *	float single_float_add(float a, float b)			*
 *									*
 *	1. Extract each of a and b into a 2's complement representation,*
 *	store in ax, bx.						*
 *	2. Determine the difference between their exponents, diffexp	*
 *	3. Invoke scale_it on bx if it has a smaller exponent.		*
 *	   or invoke scale_it on ax it it has a smaller exponent.	*
 *	   Do neither if the exponents are the same.			*
 *	4. Add the fractions as 2's complement numbers, shifting right	*
 *	to avoid overflow. Compensate the right-shift with increment of *
 *	exponent.							*
 *	5. Call normalize to normalize the result.			*
 *	6. Return float value retval;					*
 *----------------------------------------------------------------------*/


float single_float_add(float a, float b)
	{
	INTFLOAT ax, bx;
	INTFLOAT result;
	int diffexp;
	float retval;

	extract_float(&ax, a);	// get a operand into 2s complement
	extract_float(&bx, b);	// and get b operand too

	diffexp = ax.exponent - bx.exponent;	// Difference in exponents (a-b)
	if (diffexp>0) scale_it(&bx, diffexp);	// a is bigger, scale down b
	if (diffexp<0) scale_it(&ax, -diffexp);	// b is bigger, scale down a

	result.fraction = (ax.fraction >> 1) + (bx.fraction >> 1); // add the fractions
				// scale each down 1 bit to avoid overflow
	result.exponent = ax.exponent + 1;	// account for scaled down value

// Need to normalize and pack result. (NOT IMPLEMENTED HERE)

	normalize(&result);	// go normalize it

	printf("Post-normalize, single_float_add called with a=%.8f, b=%.8f\n", a, b);
	printf("result.fraction = 0x%08X, result.exponent = 0x%08X (%d)\n\n", result.fraction, result.exponent, result.exponent);
	
	}


// ok, the fun begins:

main()
  {
  INTFLOAT x;

  extract_float(&x, 2.5);
  extract_float(&x, -2.5);

// Test cases:

  single_float_add(2.5, 5.0); 	// produces 7.5  
  single_float_add(2.5, -2.5);	// produces 0.0
  single_float_add(1.0, 1.0);	// yes, 1 and 1 is 2.
  single_float_add(0.5, -2.5);	// should be -2.0
  single_float_add(2.0, -3.0);	// should be -1.0
  single_float_add(.1, .1);

// Note all of these produce 2's complement results, with the high bit being
// the sign, and the next bit being the hidden '1'. The binary point is to the
// right of the hidden 1. Note the sign bit of the fraction is a value of -2. 
// The next bit to its right is the hidden 1.  


  }
 
