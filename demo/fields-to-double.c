/* fields-to-double.c
Copyright (C) 2018 Eric Herman

gcc -pipe -g -std=c89 -Wno-long-long -Wall -Wextra -Werror -pedantic \
 -DHAVE_STDINT_H=1 -DHAVE_LIMITS_H=1 -DHAVE_FLOAT_H=1 -DHAVE_MEMCPY=1 \
 -DHAVE_STRING_H=1 -DHAVE_STDIO_H=1 -DHAVE_ERRNO=1 -DHAVE_ERRNO_H=1 \
 -I./src src/efloat.c \
 -I./demo demo/ehstr.c \
  demo/fields-to-double.c -o fields-to-double

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
	double f;
	struct efloat_double_fields fields;
	uint64_t u;
	enum efloat_class cls;
	size_t bitsbuflen = (efloat_double + 1);
	char bits[(efloat_double + 1)];

	fields.sign = argc > 1 ? atoi(argv[1]) : 1;
	fields.exponent = argc > 2 ? atoi(argv[2]) : 0;
	fields.significand = argc > 3 ? atou64(argv[3]) : 0;

	f = efloat_double_from_fields(fields, &cls);

	u = efloat64_to_uint64_bits(f);

	printf("%f as fields: sign: %d, exp: %d, mant: %lld\n",
	       f,
	       (int)fields.sign,
	       (int)fields.exponent, (unsigned long long)fields.significand);
	printf("%f bits: 0b%s\n", f, utob(bits, bitsbuflen, u, 64));
	printf("%f as unsigned: %llu\n", f, (unsigned long long)u);
	printf("%f as expression: '%d * (2^%d) * (%lld / (2^%u))'\n",
	       f,
	       (int)fields.sign,
	       (int)fields.exponent,
	       (unsigned long long)fields.significand, efloat_double_exp_shift);

	return 0;
}
