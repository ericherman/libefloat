/* u64-to-f64.c
Copyright (C) 2018 Eric Herman

uses:
https://github.com/ericherman/libefloat

gcc -pipe -g -Wall -Wextra -Werror -pedantic \
 -DHAVE_STDINT_H=1 -DHAVE_LIMITS_H=1 -DHAVE_FLOAT_H=1 -DHAVE_MEMCPY=1 \
 -DHAVE_STRING_H=1 -DHAVE_STDIO_H=1 -DHAVE_ERRNO=1 -DHAVE_ERRNO_H=1 \
 -I./src src/efloat.c demo/u64-to-f64.c -o u64-to-f64

This work is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later
version.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <stdint.h>
#include <efloat.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

int parse_and_print_u64_to_f64(const char *str, int base)
{
	uint64_t u64;
	int saved_err;

	errno = 0;
	u64 = strtoul(str, NULL, base);
	saved_err = errno;
	if (saved_err) {
		printf("error: %d %s from strtoul(%s, NULL, %d)\n", saved_err,
		       strerror(saved_err), str, base);
		return 1;
	}

	printf("%lu : %ld : %1.*e\n", (unsigned long)u64, (long)u64,
	       DECIMAL_DIG, (double)uint64_to_efloat64(u64));
	return 0;
}

int main(int argc, char **argv)
{
	int base, error, errors;
	char buf[256];

	buf[0] = '\0';
	errors = 0;

	base = argc > 2 ? atoi(argv[2]) : 10;
	if (base < 2 || base > 16) {
		return EXIT_FAILURE;
	}

	if (argc > 1) {
		error = parse_and_print_u64_to_f64(argv[1], base);
		if (error) {
			++errors;
		}
	}

	while (fgets(buf, 255, stdin) != NULL) {
		buf[255] = '\0';
		error = parse_and_print_u64_to_f64(buf, base);
		if (error) {
			++errors;
		}
	}

	return errors ? EXIT_FAILURE : EXIT_SUCCESS;
}
