#ifndef DECODER_H
#define DECODER_H

void getType(unsigned op, unsigned funct, char *type, char *functStr);
void strDecoded(unsigned, char*, unsigned);
unsigned signExtend(unsigned orig);

#endif