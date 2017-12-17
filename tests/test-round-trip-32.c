/*
test-round-trip-32.c: quick test for the Embedable Float manipulation library
Copyright (C) 2017 Eric Herman

https://github.com/ericherman/libefloat

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later
version.
*/

#include <efloat.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

unsigned round_trip32(efloat32 f)
{
	efloat32 f2;
	uint8_t sign, s2;
	int16_t exponent, exp2;
	uint32_t significand, signif2;

	enum efloat_class cls, cls2;

	cls = efloat32_radix_2_to_fields(f, &sign, &exponent, &significand);
	if (cls != fpclassify(f)) {
		fprintf(stderr, "cls %d != fpclassify (%d)\n", cls,
			fpclassify(f));
		return 1;
	}

	f2 = efloat32_radix_2_from_fields(sign, exponent, significand, &cls2);
	if (efloat32_to_uint32(f2) == efloat32_to_uint32(f)) {
		return 0;
	}

	efloat32_radix_2_to_fields(f2, &s2, &exp2, &signif2);
	fprintf(stderr, "%g (%lu) != %g (%lu)\n", f2,
		(unsigned long)efloat32_to_uint32(f2), f,
		(unsigned long)efloat32_to_uint32(f));
	fprintf(stderr, "float: %g, sign: %u, exp: %d signif: %lu\n", f,
		sign, exponent, (unsigned long)significand);
	fprintf(stderr, "float: %g, sign: %u, exp: %d signif: %lu\n", f2,
		s2, exp2, (unsigned long)signif2);
	if (cls2 != cls) {
		fprintf(stderr, "cls %d != %d\n", cls2, cls);
	}
	fprintf(stderr, "\n");
	return 1;
}

int main(int argc, char **argv)
{
	int32_t i, step;
	efloat32 f;
	int64_t err, cnt;
	int verbose;

	verbose = argc > 1 ? atoi(argv[1]) : 0;
	step = argc > 2 ? atoi(argv[2]) : 0;

	if (step <= 0) {
		step = 63;
	}

	cnt = 0;
	err = 0;
	err += round_trip32(0.0);

	err += round_trip32(uint32_to_efloat32(0));
	err += round_trip32(uint32_to_efloat32(UINT32_MAX));

	err += round_trip32(int32_to_efloat32(-1));
	err += round_trip32(int32_to_efloat32(INT32_MAX));
	err += round_trip32(int32_to_efloat32(-INT32_MAX));
	err += round_trip32(int32_to_efloat32(INT32_MIN));

	err += round_trip32(FLT_MAX);
	err += round_trip32(-FLT_MAX);
	err += round_trip32(FLT_MIN);
	err += round_trip32(-FLT_MIN);

	cnt = 11;

	for (i = 0; i >= 0 && i < INT32_MAX; i += step) {
		++cnt;
		f = int32_to_efloat32(i);
		if (i == efloat32_to_int32(f)) {
			err += round_trip32(f);
		} else {
			fprintf(stderr, "%ld fails float round trip\n",
				(long)i);
			return 1;
		}
		f = int32_to_efloat32(-i);
		if (-i == efloat32_to_int32(f)) {
			err += round_trip32(f);
		} else {
			fprintf(stderr, "%ld fails float round trip\n",
				(long)-i);
			return 1;
		}
	}
	if (verbose) {
		printf("attempted %lu values, %lu errors\n", cnt, err);
	}
	return (err == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
