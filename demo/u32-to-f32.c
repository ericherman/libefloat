/* u32-to-f32.c
Copyright (C) 2018 Eric Herman

uses:
https://github.com/ericherman/libefloat

gcc -pipe -g -Wall -Wextra -Werror -pedantic \
 -DHAVE_STDINT_H=1 -DHAVE_LIMITS_H=1 -DHAVE_FLOAT_H=1 -DHAVE_MEMCPY=1 \
 -DHAVE_STRING_H=1 -DHAVE_STDIO_H=1 -DHAVE_ERRNO=1 -DHAVE_ERRNO_H=1 \
 -I./src src/efloat.c demo/u32-to-f32.c -o u32-to-f32

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

int parse_and_print_u32_to_f32(const char *str, int base)
{
	uint32_t u32;
	int32_t i32;
	float f32;
	int saved_err;

	errno = 0;
	u32 = strtoul(str, NULL, base);
	saved_err = errno;
	if (saved_err) {
		printf("error: %d %s from strtoul(%s, NULL, %d)\n", saved_err,
		       strerror(saved_err), str, base);
		return 1;
	}
	i32 = (int32_t)u32;
	f32 = uint32_to_efloat32(u32);
	/*
	printf("%u : %d : %1.*e\n", (unsigned)u32, (int)i32,
	       DECIMAL_DIG, f32);
	*/
	printf("%u : %d : %g\n", (unsigned)u32, (int)i32, f32);
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
		error = parse_and_print_u32_to_f32(argv[1], base);
		if (error) {
			++errors;
		}
	}

	while (fgets(buf, 255, stdin) != NULL) {
		buf[255] = '\0';
		error = parse_and_print_u32_to_f32(buf, base);
		if (error) {
			++errors;
		}
	}

	return errors ? EXIT_FAILURE : EXIT_SUCCESS;
}
