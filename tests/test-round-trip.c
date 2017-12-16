/*
test-round-trip.c: quick test for the Embedable Float manipulation library
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

unsigned round_trip64(efloat64 f)
{
	efloat64 f2;
	uint8_t sign, s2;
	int16_t exponent, exp2;
	uint64_t fraction, frac2;

	enum efloat_class cls, cls2;

	cls = efloat64_radix_2_to_fields(f, &sign, &exponent, &fraction);
	if (cls != fpclassify(f)) {
		fprintf(stderr, "cls %d != fpclassify (%d)\n", cls,
			fpclassify(f));
		return 1;
	}

	f2 = efloat64_radix_2_from_fields(sign, exponent, fraction, &cls2);
	if (efloat64_to_uint64(f2) == efloat64_to_uint64(f)) {
		return 0;
	}

	efloat64_radix_2_to_fields(f2, &s2, &exp2, &frac2);
	fprintf(stderr, "%g (%lu) != %g (%lu)\n", f2,
		(unsigned long)efloat64_to_uint64(f2), f,
		(unsigned long)efloat64_to_uint64(f));
	fprintf(stderr, "float: %g, sign: %u, exp: %d frac: %lu\n", f,
		sign, exponent, fraction);
	fprintf(stderr, "float: %g, sign: %u, exp: %d frac: %lu\n", f2,
		s2, exp2, frac2);
	if (cls2 != cls) {
		fprintf(stderr, "cls %d != %d\n", cls2, cls);
	}
	fprintf(stderr, "\n");
	return 1;
}

int main(int argc, char **argv)
{
	int64_t i;
	efloat64 f;
	size_t err, cnt;
	int verbose;

	verbose = argc > 1 ? atoi(argv[1]) : 0;
	cnt = 0;
	err = 0;
	err += round_trip64(0.0);

	err += round_trip64(uint64_to_efloat64(0));
	err += round_trip64(uint64_to_efloat64(UINT64_MAX));

	err += round_trip64(int64_to_efloat64(-1));
	err += round_trip64(int64_to_efloat64(INT64_MAX));
	err += round_trip64(int64_to_efloat64(-INT64_MAX));
	err += round_trip64(int64_to_efloat64(INT64_MIN));

	err += round_trip64(DBL_MAX);
	err += round_trip64(-DBL_MAX);
	err += round_trip64(DBL_MIN);
	err += round_trip64(-DBL_MIN);

	cnt = 11;

	for (i = 0; i >= 0 && i < INT64_MAX; i += (100 * (int64_t)INT32_MAX)) {
		++cnt;
		f = int64_to_efloat64(i);
		if (i == efloat64_to_int64(f)) {
			err += round_trip64(f);
		} else {
			fprintf(stderr, "%ld fails float round trip\n",
				(long)i);
			return 1;
		}
		f = int64_to_efloat64(-i);
		if (-i == efloat64_to_int64(f)) {
			err += round_trip64(f);
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
