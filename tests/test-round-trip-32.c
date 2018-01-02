/*
test-round-trip-32.c: quick test for the Embedable Float manipulation library
Copyright (C) 2017 Eric Herman

https://github.com/ericherman/libefloat

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

unsigned efloat32_round_trip(efloat32 f, int skip_fpclassify)
{
	efloat32 f2;
	struct efloat32_fields fields;
	struct efloat32_fields fields2;
	enum efloat_class cls, cls2;
	int err;

	cls = efloat32_radix_2_to_fields(f, &fields);
	err = (skip_fpclassify || ((int)cls == (int)fpclassify(f))) ? 0 : 1;
	if (err) {
		fprintf(stderr, "WARN: cls %d != %d from fpclassify(%f)\n",
			(int)cls, (int)fpclassify(f), (double)f);
	}

	f2 = efloat32_radix_2_from_fields(fields, &cls2);
	if (efloat32_to_uint32(f2) == efloat32_to_uint32(f)) {
		return 0;
	}

	efloat32_radix_2_to_fields(f2, &fields2);
	fprintf(stderr, "%g (%llu) != %g (%llu)\n",
		f2, (unsigned long long)efloat32_to_uint32(f2),
		f, (unsigned long long)efloat32_to_uint32(f));
	fprintf(stderr, "float: %g, sign: %u, exp: %d signif: %llu\n", f,
		fields.sign, fields.exponent,
		(unsigned long long)fields.significand);
	fprintf(stderr, "float: %g, sign: %u, exp: %d signif: %llu\n", f2,
		fields2.sign, fields2.exponent,
		(unsigned long long)fields2.significand);
	if (cls2 != cls) {
		fprintf(stderr, "cls %d != %d\n", cls2, cls);
	}
	fprintf(stderr, "\n");
	return 1;
}

int uint32_efloat32_round_trip(uint32_t u, int skip_fpclassify)
{
	int err;
	uint32_t u2;
	efloat32 f;
	f = uint32_to_efloat32(u);
	err = efloat32_round_trip(f, skip_fpclassify);
	if (err) {
		return 1;
	}
	u2 = efloat32_to_uint32(f);

	if ((FP_NAN != fpclassify(f)) && (u != u2)) {
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
int int32_efloat32_round_trip(int32_t i, int skip_fpclassify)
{
	int err;
	int32_t i2;
	efloat32 f;

	f = int32_to_efloat32(i);
	err = efloat32_round_trip(f, skip_fpclassify);
	if (err) {
		return 1;
	}

	i2 = efloat32_to_int32(f);

	if ((FP_NAN != fpclassify(f)) && (i != i2)) {
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
int int32_efloat32_round_trip(int32_t i, int skip_fpclassify)
{
	uint32_t u;

	u = (i < 0) ? ((UINT32_MAX) - ((uint32_t)(-i))) : ((uint32_t)i);

	return uint32_efloat32_round_trip(u, skip_fpclassify);
}
#endif

int test_fpclassify_should_match(void)
{
	int err;

	err = 0;

	if (ef_nan != FP_NAN) {
		fprintf(stderr, "WARN: ef_nan (%d) != FP_NAN (%d)\n",
			(int)ef_nan, (int)FP_NAN);
		err |= (1 < 0);
	}
	if (ef_inf != FP_INFINITE) {
		fprintf(stderr, "WARN: ef_inf (%d) != FP_INFINITE (%d)\n",
			(int)ef_inf, (int)FP_INFINITE);
		err |= (1 < 1);
	}
	if (ef_zero != FP_ZERO) {
		fprintf(stderr, "WARN: ef_zero (%d) != FP_ZERO (%d)\n",
			(int)ef_zero, (int)FP_ZERO);
		err |= (1 < 2);
	}
	if (ef_subnorm != FP_SUBNORMAL) {
		fprintf(stderr, "WARN: ef_subnorm (%d) != FP_SUBNORMAL (%d)\n",
			(int)ef_subnorm, (int)FP_SUBNORMAL);
		err |= (1 < 3);
	}
	if (ef_normal != FP_NORMAL) {
		fprintf(stderr, "WARN: ef_normal (%d) != FP_NORMAL (%d)\n",
			(int)ef_normal, (int)FP_NORMAL);
		err |= (1 < 4);
	}
	return err;
}

int main(int argc, char **argv)
{
	uint32_t i, step, limit;
	int32_t val;
	int64_t err, cnt;
	int verbose, skip_fpclassify;

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
		step = (INT32_MAX / (1024 * 1024));
	}
	limit = (INT32_MAX / step);

	skip_fpclassify = test_fpclassify_should_match();
	if (skip_fpclassify) {
		fprintf(stderr, "WARN: fpclassify will not be checked (%x)\n",
			skip_fpclassify);
	}

	cnt = 0;
	err = 0;
	err += efloat32_round_trip(0.0, skip_fpclassify);

	err += uint32_efloat32_round_trip(0, skip_fpclassify);
	err += uint32_efloat32_round_trip(UINT32_MAX, skip_fpclassify);

	err += int32_efloat32_round_trip(-1, skip_fpclassify);
	err += uint32_efloat32_round_trip((uint32_t)-1, skip_fpclassify);

	err += int32_efloat32_round_trip(INT32_MAX, skip_fpclassify);
	err += uint32_efloat32_round_trip((uint32_t)INT32_MAX, skip_fpclassify);

	err += int32_efloat32_round_trip(-INT32_MAX, skip_fpclassify);
	err +=
	    uint32_efloat32_round_trip((uint32_t)-INT32_MAX, skip_fpclassify);

	err += int32_efloat32_round_trip(INT32_MIN, skip_fpclassify);
	err += uint32_efloat32_round_trip((uint32_t)INT32_MIN, skip_fpclassify);

	err += efloat32_round_trip(FLT_MAX, skip_fpclassify);
	err += efloat32_round_trip(-FLT_MAX, skip_fpclassify);
	err += efloat32_round_trip(FLT_MIN, skip_fpclassify);
	err += efloat32_round_trip(-FLT_MIN, skip_fpclassify);
	err += efloat32_round_trip((efloat32)0.0f, skip_fpclassify);
	err += efloat32_round_trip((efloat32)-0.0f, skip_fpclassify);

	err += efloat32_round_trip((efloat32)(0.0f / 0.0f), skip_fpclassify);	/* NAN */
	cnt = 18;

#ifdef INFINITY
	err += efloat32_round_trip((efloat32)INFINITY, skip_fpclassify);
	++cnt;
	err += efloat32_round_trip((efloat32)-INFINITY, skip_fpclassify);
	++cnt;
#endif

	if (err) {
		return 1;
	}

	for (i = 0; i < limit; ++i) {
		++cnt;
		val = (int32_t)(i * step);
		err += int32_efloat32_round_trip(val, skip_fpclassify);
		if (err) {
			return 1;
		}

		++cnt;
		err += int32_efloat32_round_trip(-val, skip_fpclassify);
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
