/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* double-to-fields.c */
/* Copyright (C) 2018, 2019 Eric Herman */
/* https://github.com/ericherman/libefloat */
/*
   gcc -pipe -g -O2 -std=gnu89 -pedantic -Wno-long-long -Werror -Wall -Wextra \
    -Werror=cast-qual -fomit-frame-pointer -DNDEBUG \
    -I./submodules/libehstr/src ./submodules/libehstr/src/ehstr.c \
    -I./submodules/libecheck/src ./submodules/libecheck/src/eembed.c \
    -I./src src/efloat.c \
    -I./demo demo/double-to-fields.c -o demo/double-to-fields
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
