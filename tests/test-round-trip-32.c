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

unsigned efloat32_round_trip(efloat32 f)
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
	fprintf(stderr, "%g (%llu) != %g (%llu)\n",
		f2, (unsigned long long)efloat32_to_uint32(f2),
		f, (unsigned long long)efloat32_to_uint32(f));
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

int uint32_efloat32_round_trip(uint32_t u)
{
	int err;
	uint32_t u2;
	efloat32 f;

	f = uint32_to_efloat32(u);
	err = efloat32_round_trip(f);
	if (err) {
		return 1;
	}
	u2 = efloat32_to_uint32(f);

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
int int32_efloat32_round_trip(int32_t i)
{
	int err;
	int32_t i2;
	efloat32 f;

	f = int32_to_efloat32(i);
	err = efloat32_round_trip(f);
	if (err) {
		return 1;
	}

	i2 = efloat32_to_int32(f);

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
int int32_efloat32_round_trip(int32_t i)
{
	uint32_t u;

	u = (i < 0) ? ((UINT32_MAX) - ((uint32_t)(-i))) : ((uint32_t)i);

	return uint32_efloat32_round_trip(u);
}
#endif

int main(int argc, char **argv)
{
	int32_t i, step, limit;
	int64_t err, cnt;
	int verbose;

	if (sizeof(efloat32) != sizeof(int32_t)) {
		fprintf(stderr,
			"sizeof(efloat32) %lu != sizeof(int32_t) %lu!\n",
			(unsigned long)sizeof(efloat32),
			(unsigned long)sizeof(int32_t));
		return EXIT_FAILURE;
	}

	verbose = argc > 1 ? atoi(argv[1]) : 0;
	step = argc > 2 ? atoi(argv[2]) : 0;

	if (step <= 0) {
		step = (INT32_MAX / (32 * 1024));
	}
	limit = (INT32_MAX / step);

	cnt = 0;
	err = 0;
	err += efloat32_round_trip(0.0);

	err += uint32_efloat32_round_trip(0);
	err += uint32_efloat32_round_trip(UINT32_MAX);

	err += int32_efloat32_round_trip(-1);
	err += uint32_efloat32_round_trip((uint32_t)-1);

	err += int32_efloat32_round_trip(INT32_MAX);
	err += uint32_efloat32_round_trip((uint32_t)INT32_MAX);

	err += int32_efloat32_round_trip(-INT32_MAX);
	err += uint32_efloat32_round_trip((uint32_t)-INT32_MAX);

	err += int32_efloat32_round_trip(INT32_MIN);
	err += uint32_efloat32_round_trip((uint32_t)INT32_MIN);

	err += efloat32_round_trip(FLT_MAX);
	err += efloat32_round_trip(-FLT_MAX);
	err += efloat32_round_trip(FLT_MIN);
	err += efloat32_round_trip(-FLT_MIN);

	cnt = 15;
	if (err) {
		return 1;
	}

	for (i = 0; i < limit; ++i) {
		++cnt;
		err += int32_efloat32_round_trip(i * step);
		if (err) {
			return 1;
		}

		++cnt;
		err += int32_efloat32_round_trip(-(i * step));
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
