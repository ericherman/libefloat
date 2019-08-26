/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* test-round-trip-64.c: test of the Embedable Float manipulation library */
/* Copyright (C) 2017, 2018, 2019 Eric Herman */
/* https://github.com/ericherman/libefloat */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <stdint.h>
#include <efloat.h>

int check_fpclassify(enum efloat_class cls, efloat64 f)
{
	int err = 0;
	const char *fpstr;
	char *ef_strs[] =
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

unsigned efloat64_round_trip(efloat64 f)
{
	efloat64 f2;
	struct efloat64_fields fields;
	struct efloat64_fields fields2;
	enum efloat_class cls, cls2;

	cls = efloat64_radix_2_to_fields(f, &fields);
	check_fpclassify(cls, f);

	f2 = efloat64_radix_2_from_fields(fields, &cls2);
	if (efloat64_to_uint64_bits(f2) == efloat64_to_uint64_bits(f)) {
		return 0;
	}

	efloat64_radix_2_to_fields(f2, &fields2);
	fprintf(stderr, "%g (%llu) != %g (%llu)\n",
		f2, (unsigned long long)efloat64_to_uint64_bits(f2),
		f, (unsigned long long)efloat64_to_uint64_bits(f));
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

int uint64_efloat64_round_trip(uint64_t u)
{
	int err;
	uint64_t u2;
	efloat64 f;
	f = uint64_bits_to_efloat64(u);
	err = efloat64_round_trip(f);
	if (err) {
		return 1;
	}
	u2 = efloat64_to_uint64_bits(f);

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
int int64_efloat64_round_trip(int64_t i)
{
	int err;
	int64_t i2;
	efloat64 f;

	f = int64_bits_to_efloat64(i);
	err = efloat64_round_trip(f);
	if (err) {
		return 1;
	}

	i2 = efloat64_to_int64_bits(f);

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
		step = (INT64_MAX / (1024 * 1024));
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

	err += efloat64_round_trip(DBL_MAX);
	err += efloat64_round_trip(-DBL_MAX);
	err += efloat64_round_trip(DBL_MIN);
	err += efloat64_round_trip(-DBL_MIN);
	err += efloat64_round_trip((efloat64)0.0f);
	err += efloat64_round_trip((efloat64)-0.0f);

	err += efloat64_round_trip((efloat64)(0.0f / 0.0f));	/* NAN */
	cnt = 18;

#ifdef INFINITY
	err += efloat64_round_trip((efloat64)INFINITY);
	++cnt;
	err += efloat64_round_trip((efloat64)-INFINITY);
	++cnt;
#endif

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
