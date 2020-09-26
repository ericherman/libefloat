/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* float-to-fields.c */
/* Copyright (C) 2018, 2019 Eric Herman */
/* https://github.com/ericherman/libefloat */
/*
   gcc -pipe -g -O2 -std=gnu89 -pedantic -Wno-long-long -Werror -Wall -Wextra \
    -Werror=cast-qual -fomit-frame-pointer -DNDEBUG \
    -DHAVE_ERRNO=1 -DHAVE_ERRNO_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_MEMCPY=1 \
    -DHAVE_STRING_H=1 -DHAVE_SATDIO_H=1 -DHAVE_SNPRINTF=1 \
    -I./src -I./submodules/libehstr/src -I./demo \
    src/efloat.c ./submodules/libehstr/src/ehstr.c \
    demo/float-to-fields.c -o float-to-fields
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
	size_t buflen = 80;
	char buf[80];

	buf[0] = '\0';

	f = argc > 1 ? atof(argv[1]) : 2.0;

	u = efloat32_to_uint32_bits(f);

	efloat_float_to_fields(f, &fields);

	printf("%f bits: 0b%s\n", f, utob(buf, buflen, u, efloat_float));
	printf("%f as unsigned: %lu\n", f, (unsigned long)u);
	printf("%f as fields: sign: %d, exp: %d, mant: %ld\n",
	       f,
	       (int)fields.sign,
	       (int)fields.exponent, (unsigned long)fields.significand);
	printf("%f as expression: '%s'\n",
	       f, efloat_float_fields_to_expression(fields, buf, buflen, NULL));

	return 0;
}
