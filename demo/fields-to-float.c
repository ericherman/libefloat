/* fields-to-float.c
Copyright (C) 2018 Eric Herman

gcc -pipe -g -std=c89 -Wno-long-long -Wall -Wextra -Werror -pedantic \
 -DHAVE_STDINT_H=1 -DHAVE_LIMITS_H=1 -DHAVE_FLOAT_H=1 -DHAVE_MEMCPY=1 \
 -DHAVE_STRING_H=1 -DHAVE_STDIO_H=1 -DHAVE_ERRNO=1 -DHAVE_ERRNO_H=1 \
 -I./src src/efloat.c \
 -I./demo demo/ehstr.c \
  demo/fields-to-float.c -o fields-to-float

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
#include <values.h>
#include "ehstr.h"
#include "efloat.h"

#if(UINT_MAX == 4294967295UL)
#define atou32(str) ((uint32_t)strtoul(str, NULL, 10))
#else
#define atou32(str) ((uint32_t)strtoull(str, NULL, 10))
#endif

#if(ULONG_MAX == 4294967295UL)
#define atou64(str) ((uint64_t)strtoull(str, NULL, 10))
#else
#define atou64(str) ((uint64_t)strtoul(str, NULL, 10))
#endif

int main(int argc, const char **argv)
{
	float f;
	struct efloat_float_fields fields;
	uint32_t u;

	enum efloat_class cls;
	size_t bitsbuflen = (efloat_float + 1);
	char bits[(efloat_float + 1)];

	fields.sign = argc > 1 ? atoi(argv[1]) : 1;
	fields.exponent = argc > 2 ? atoi(argv[2]) : 0;
	fields.significand = argc > 3 ? atou32(argv[3]) : 0;

	f = efloat_float_from_fields(fields, &cls);

	u = efloat32_to_uint32_bits(f);

	printf("%f as fields: sign: %d, exp: %d, mant: %lld\n",
	       f,
	       (int)fields.sign,
	       (int)fields.exponent, (unsigned long long)fields.significand);
	printf("%f bits: 0b%s\n", f, utob(bits, bitsbuflen, u, efloat_float));
	printf("%f as unsigned: %llu\n", f, (unsigned long long)u);
	printf("%f as expression: '%d * (2^%d) * (%lld / (2^%u))'\n",
	       f,
	       (int)fields.sign,
	       (int)fields.exponent,
	       (unsigned long long)fields.significand, efloat_float_exp_shift);

	return 0;
}
