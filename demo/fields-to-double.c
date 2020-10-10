/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* fields-to-double.c */
/* Copyright (C) 2018, 2019 Eric Herman */
/* https://github.com/ericherman/libefloat */
/*
   gcc -pipe -g -O2 -std=gnu89 -pedantic -Wno-long-long -Werror -Wall -Wextra \
    -Werror=cast-qual -fomit-frame-pointer -DNDEBUG \
    -I./src -I./submodules/libehstr/src \
    -I./submodules/libecheck/src -I./demo \
    src/efloat.c ./submodules/libehstr/src/ehstr.c \
    ./submodules/libecheck/src/eembed.c \
    demo/fields-to-double.c -o fields-to-double
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
	size_t buflen = 80;
	char buf[80];

	buf[0] = '\0';

	fields.sign = argc > 1 ? atoi(argv[1]) : 1;
	fields.exponent = argc > 2 ? atoi(argv[2]) : 0;
	fields.significand = argc > 3 ? atou64(argv[3]) : 0;

	f = efloat_double_from_fields(fields, &cls);

	u = efloat64_to_uint64_bits(f);

	printf("%f as fields: sign: %d, exp: %d, mant: %lld\n",
	       f,
	       (int)fields.sign,
	       (int)fields.exponent, (unsigned long long)fields.significand);
	printf("%f bits: 0b%s\n", f, utob(buf, buflen, u, efloat_double));
	printf("%f as unsigned: %llu\n", f, (unsigned long long)u);
	printf("%f as expression: '%s'\n",
	       f,
	       efloat_double_fields_to_expression(fields, buf, buflen, NULL));

	return 0;
}
