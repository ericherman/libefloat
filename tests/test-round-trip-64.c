/*
test-round-trip-64.c: quick test for the Embedable Float manipulation library
Copyright (C) 2017 Eric Herman

https://github.com/ericherman/libefloat

This work is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later
version.
*/

#include <efloat.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

unsigned efloat64_round_trip(efloat64 f)
{
	efloat64 f2;
	uint8_t sign, s2;
	int16_t exponent, exp2;
	uint64_t significand, signif2;

	enum efloat_class cls, cls2;

	cls = efloat64_radix_2_to_fields(f, &sign, &exponent, &significand);
	if (cls != fpclassify(f)) {
		fprintf(stderr, "cls %d != fpclassify (%d)\n", cls,
			fpclassify(f));
		return 1;
	}

	f2 = efloat64_radix_2_from_fields(sign, exponent, significand, &cls2);
	if (efloat64_to_uint64(f2) == efloat64_to_uint64(f)) {
		return 0;
	}

	efloat64_radix_2_to_fields(f2, &s2, &exp2, &signif2);
	fprintf(stderr, "%g (%llu) != %g (%llu)\n",
		f2, (unsigned long long)efloat64_to_uint64(f2),
		f, (unsigned long long)efloat64_to_uint64(f));
	fprintf(stderr, "float: %g, sign: %u, exp: %d signif: %llu\n", f,
		sign, exponent, (unsigned long long)significand);
	fprintf(stderr, "float: %g, sign: %u, exp: %d signif: %llu\n", f2,
		s2, exp2, (unsigned long long)signif2);
	if (cls2 != cls) {
		fprintf(stderr, "cls %d != %d\n", cls2, cls);
	}
	fprintf(stderr, "\n");
	return 1;
}

int uint64_efloat64_round_trip(uint64_t u)
{
	int err;
	uint64_t u2;
	efloat64 f;

	f = uint64_to_efloat64(u);
	err = efloat64_round_trip(f);
	if (err) {
		return 1;
	}
	u2 = efloat64_to_uint64(f);

	if (u != u2) {
		fprintf(stderr,
			"%llu fails uint-float-uint round trip as:\n%llu\n",
			(unsigned long long)u, (unsigned long long)u2);
		fprintf(stderr,
			"0x%0llX fails uint-float-uint round trip as:\n0x%0llX\n",
			(unsigned long long)u, (unsigned long long)u2);
		return 1;
	}

	return 0;
}

#if ((defined efloat64_also_signed_ints) && (efloat64_also_signed_ints))
int int64_efloat64_round_trip(int64_t i)
{
	int err;
	int64_t i2;
	efloat64 f;

	f = int64_to_efloat64(i);
	err = efloat64_round_trip(f);
	if (err) {
		return 1;
	}

	i2 = efloat64_to_int64(f);

	if (i != i2) {
		fprintf(stderr,
			"%lld fails int-float-int round trip as:\n%lld\n",
			(long long)i, (long long)i2);
		fprintf(stderr,
			"0x%0llX fails int-float-int round trip as:\n0x%0llX\n",
			(long long)i, (long long)i2);
		return 1;
	}

	return 0;
}
#else
int int64_efloat64_round_trip(int64_t i)
{
	uint64_t u;

	u = (i < 0) ? ((UINT64_MAX) - ((uint64_t)(-i))) : ((uint64_t)i);

	return uint64_efloat64_round_trip(u);
}
#endif

int main(int argc, char **argv)
{
	uint64_t i, step, limit;
	int64_t val;
	int64_t err, cnt;
	int verbose;

	if (sizeof(efloat64) != sizeof(int64_t)) {
		fprintf(stderr,
			"sizeof(efloat64) %lu != sizeof(int64_t) %lu!\n",
			(unsigned long)sizeof(efloat64),
			(unsigned long)sizeof(int64_t));
		return EXIT_FAILURE;
	}

	verbose = argc > 1 ? atoi(argv[1]) : 0;
	step = argc > 2 ? atoi(argv[2]) : 0;

	if (step <= 0) {
		step = (INT64_MAX / (64 * 1024));
	}
	limit = (INT64_MAX / step);

	cnt = 0;
	err = 0;
	err += efloat64_round_trip(0.0);

	err += uint64_efloat64_round_trip(0);
	err += uint64_efloat64_round_trip(UINT64_MAX);

	err += int64_efloat64_round_trip(-1);
	err += uint64_efloat64_round_trip((uint64_t)-1);

	err += int64_efloat64_round_trip(INT64_MAX);
	err += uint64_efloat64_round_trip((uint64_t)INT64_MAX);

	err += int64_efloat64_round_trip(-INT64_MAX);
	err += uint64_efloat64_round_trip((uint64_t)-INT64_MAX);

	err += int64_efloat64_round_trip(INT64_MIN);
	err += uint64_efloat64_round_trip((uint64_t)INT64_MIN);

	err += efloat64_round_trip(FLT_MAX);
	err += efloat64_round_trip(-FLT_MAX);
	err += efloat64_round_trip(FLT_MIN);
	err += efloat64_round_trip(-FLT_MIN);

	cnt = 15;
	if (err) {
		return 1;
	}

	for (i = 0; i < limit; ++i) {
		++cnt;
		val = (int64_t)(i * step);
		err += int64_efloat64_round_trip(val);
		if (err) {
			return 1;
		}

		++cnt;
		err += int64_efloat64_round_trip(-val);
		if (err) {
			return 1;
		}
	}
	if (verbose) {
		printf("attempted %llu values, %llu errors\n",
		       (unsigned long long)cnt, (unsigned long long)err);
	}
	return (err == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
