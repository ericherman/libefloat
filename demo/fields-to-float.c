/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* fields-to-float.c */
/* Copyright (C) 2018, 2019 Eric Herman */
/* https://github.com/ericherman/libefloat */
/*
gcc -pipe -g -std=c89 -Wno-long-long -Wall -Wextra -Werror -pedantic \
 -DHAVE_STDINT_H=1 -DHAVE_LIMITS_H=1 -DHAVE_FLOAT_H=1 -DHAVE_MEMCPY=1 \
 -DHAVE_STRING_H=1 -DHAVE_STDIO_H=1 -DHAVE_ERRNO=1 -DHAVE_ERRNO_H=1 \
 -I./src src/efloat.c \
 -I./demo demo/ehstr.c \
  demo/fields-to-float.c -o fields-to-float
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
	size_t buflen = 80;
	char buf[80];

	buf[0] = '\0';

	fields.sign = argc > 1 ? atoi(argv[1]) : 1;
	fields.exponent = argc > 2 ? atoi(argv[2]) : 0;
	fields.significand = argc > 3 ? atou32(argv[3]) : 0;

	f = efloat_float_from_fields(fields, &cls);

	u = efloat32_to_uint32_bits(f);

	printf("%f as fields: sign: %d, exp: %d, mant: %lld\n",
	       f,
	       (int)fields.sign,
	       (int)fields.exponent, (unsigned long long)fields.significand);
	printf("%f bits: 0b%s\n", f, utob(buf, buflen, u, efloat_float));
	printf("%f as unsigned: %llu\n", f, (unsigned long long)u);
	printf("%f as expression: '%s'\n",
	       f, efloat_float_fields_to_expression(fields, buf, buflen, NULL));

	return 0;
}
