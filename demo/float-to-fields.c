/* float-to-fields.c
Copyright (C) 2018 Eric Herman

gcc -pipe -g -std=c89 -Wno-long-long -Wall -Wextra -Werror -pedantic \
 -DHAVE_STDINT_H=1 -DHAVE_LIMITS_H=1 -DHAVE_FLOAT_H=1 -DHAVE_MEMCPY=1 \
 -DHAVE_STRING_H=1 -DHAVE_STDIO_H=1 -DHAVE_ERRNO=1 -DHAVE_ERRNO_H=1 \
 -I./src src/efloat.c \
 -I./demo demo/ehstr.c \
  demo/float-to-fields.c -o float-to-fields

uses:
https://github.com/ericherman/libefloat

This work is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later
version.
*/

#include <stdio.h>
#include <stdlib.h>
#include "efloat.h"
#include "ehstr.h"

int main(int argc, const char **argv)
{
	float f;
	struct efloat_float_fields fields;
	uint32_t u;
	size_t bitsbuflen = (32 + 1);
	char bits[32 + 1];

	f = argc > 1 ? atof(argv[1]) : 2.0;

	u = efloat32_to_uint32_bits(f);

	efloat_float_to_fields(f, &fields);

	printf("%f bits: 0b%s\n", f, utob(bits, bitsbuflen, u, 32));
	printf("%f as unsigned: %lu\n", f, (unsigned long)u);
	printf("%f as fields: sign: %d, exp: %d, mant: %ld\n",
	       f,
	       (int)fields.sign,
	       (int)fields.exponent, (unsigned long)fields.significand);
	printf("%f as expression: '%d * (2^%d) * (%ld / (2^%u))'\n",
	       f,
	       (int)fields.sign,
	       (int)fields.exponent,
	       (unsigned long)fields.significand, efloat_float_exp_shift);

	return 0;
}
