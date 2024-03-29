/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* test-round-trip-32.c: test of the Embedable Float manipulation library */
/* Copyright (C) 2017, 2018, 2019 Eric Herman */
/* https://github.com/ericherman/libefloat */

#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "efloat.h"

int check_fpclassify(enum efloat_class cls, efloat32 f)
{
	int err = 0;
	const char *fpstr;
	const char *ef_strs[] =
	    { "ef_nan", "ef_inf", "ef_zero", "ef_subnorm", "ef_normal" };

	switch (cls) {
	case ef_nan:
		err = (fpclassify(f) != FP_NAN);
		break;
	case ef_inf:
		err = (fpclassify(f) != FP_INFINITE);
		break;
	case ef_zero:
		err = (fpclassify(f) != FP_ZERO);
		break;
	case ef_subnorm:
		err = (fpclassify(f) != FP_SUBNORMAL);
		break;
	case ef_normal:
		err = (fpclassify(f) != FP_NORMAL);
		break;
	}
	if (err) {
		switch (fpclassify(f)) {
		case FP_NAN:
			fpstr = "FP_NAN";
			break;
		case FP_INFINITE:
			fpstr = "FP_INFINITE";
			break;
		case FP_ZERO:
			fpstr = "FP_ZERO";
			break;
		case FP_SUBNORMAL:
			fpstr = "FP_SUBNORMAL";
			break;
		case FP_NORMAL:
			fpstr = "FP_NORMAL";
			break;
		default:
			fpstr = "(UNKNOWN)";
			break;
		}

		fprintf(stderr, "cls %s (%d) != %s (%d) from fpclassify(%f)\n",
			ef_strs[cls], (int)cls, fpstr, (int)fpclassify(f),
			(double)f);
	}
	return err;
}

unsigned efloat32_round_trip(efloat32 f)
{
	efloat32 f2;
	struct efloat32_fields fields;
	struct efloat32_fields fields2;
	enum efloat_class cls, cls2;

	cls = efloat32_radix_2_to_fields(f, &fields);
	check_fpclassify(cls, f);

	f2 = efloat32_radix_2_from_fields(fields, &cls2);
	if (efloat32_to_uint32_bits(f2) == efloat32_to_uint32_bits(f)) {
		return 0;
	}

	efloat32_radix_2_to_fields(f2, &fields2);
	fprintf(stderr, "%g (%llu) != %g (%llu)\n",
		f2, (unsigned long long)efloat32_to_uint32_bits(f2),
		f, (unsigned long long)efloat32_to_uint32_bits(f));
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

int uint32_efloat32_round_trip(uint32_t u)
{
	int err;
	uint32_t u2;
	efloat32 f;
	f = uint32_bits_to_efloat32(u);
	err = efloat32_round_trip(f);
	if (err) {
		return 1;
	}
	u2 = efloat32_to_uint32_bits(f);

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

int int32_efloat32_round_trip(int32_t i)
{
	int err;
	int32_t i2;
	efloat32 f;

	f = int32_bits_to_efloat32(i);
	err = efloat32_round_trip(f);
	if (err) {
		return 1;
	}

	i2 = efloat32_to_int32_bits(f);

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

int main(int argc, char **argv)
{
	uint32_t i, step, limit;
	int32_t val;
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
		step = (INT32_MAX / (1024 * 1024));
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
	err += efloat32_round_trip((efloat32)0.0f);
	err += efloat32_round_trip((efloat32)-0.0f);

	err += efloat32_round_trip((efloat32)(0.0f / 0.0f));	/* NAN */
	cnt = 18;

#ifdef INFINITY
	err += efloat32_round_trip((efloat32)INFINITY);
	++cnt;
	err += efloat32_round_trip((efloat32)-INFINITY);
	++cnt;
#endif

	if (err) {
		return 1;
	}

	for (i = 0; i < limit; ++i) {
		++cnt;
		val = (int32_t)(i * step);
		err += int32_efloat32_round_trip(val);
		if (err) {
			return 1;
		}

		++cnt;
		err += int32_efloat32_round_trip(-val);
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
