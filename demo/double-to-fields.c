/* double-to-fields.c
Copyright (C) 2018 Eric Herman

gcc -pipe -g -std=c89 -Wno-long-long -Wall -Wextra -Werror -pedantic \
 -DHAVE_STDINT_H=1 -DHAVE_LIMITS_H=1 -DHAVE_FLOAT_H=1 -DHAVE_MEMCPY=1 \
 -DHAVE_STRING_H=1 -DHAVE_STDIO_H=1 -DHAVE_ERRNO=1 -DHAVE_ERRNO_H=1 \
 -I./src src/efloat.c \
 -I./demo demo/ehstr.c \
  demo/double-to-fields.c -o double-to-fields

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
	double f;
	struct efloat_double_fields fields;
	uint64_t u;
	size_t buflen = 80;
	char buf[80];

	f = argc > 1 ? atof(argv[1]) : 2.0;

	u = efloat64_to_uint64_bits(f);

	efloat_double_to_fields(f, &fields);

	printf("%f bits: 0b%s\n", f, utob(buf, buflen, u, efloat_double));
	printf("%f as unsigned: %llu\n", f, (unsigned long long)u);
	printf("%f as fields: sign: %d, exp: %d, mant: %lld\n",
	       f,
	       (int)fields.sign,
	       (int)fields.exponent, (unsigned long long)fields.significand);
	printf("%f as expression: '%s'\n",
	       f,
	       efloat_double_fields_to_expression(fields, buf, buflen, NULL));

	return 0;
}
