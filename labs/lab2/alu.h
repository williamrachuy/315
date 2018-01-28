#ifndef ALU_H
#define ALU_H

typedef struct {
   unsigned int sign;
   int exponent;
   int fraction;
} intFloat;

unsigned int convFltTo754();
float conv754toFlt();
unsigned int uMultiply();
long int sMultiply();
intFloat *extFloat();
float packFloat();
intFloat *scaleFloat();
intFloat *normalizeFloat();
float addFloat();
float subtractFloat();
float fmul();

#endif