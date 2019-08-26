/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* efloat.c: Embedable Float manipulation library */
/* Copyright (C) 2017, 2018, 2019 Eric Herman */
/* https://github.com/ericherman/libefloat */

#include <efloat.h>

#ifndef eprintf2
#if HAVE_STDIO_H
#include <stdio.h>
#define eprintf2(fmt, a, b) fprintf(stderr, fmt, a, b)
#else
#define eprintf2(fmt, a, b)	/* NOOP */
#endif
#endif

#ifndef HAVE_MEMCPY
#define HAVE_MEMCPY 0
#endif
#ifndef HAVE_STRING_H
#define HAVE_STRING_H 0
#endif
#if (HAVE_MEMCPY && HAVE_STRING_H)
#include <string.h>		/* memcpy */
#endif
/*
   If no memcpy(), then we can use type-punning via a union.
   The C standard reads ambuguously to me as to whether this is strictly
   conforming, but I know of no compilers which do not support it.

   The calls to memcpy() here are probably going to be removed by gcc:
   gcc output: https://godbolt.org/g/hiX8oL
*/

#if HAVE_ERRNO_H
#include <errno.h>
#define seterrnoinval() do { errno = EINVAL; } while (0)
#endif

#ifndef seterrnoinval
#define seterrnoinval()		/* NOOP */
#endif

#if HAVE_STDIO_H
#if HAVE_INTTYPES_H
#include <inttypes.h>
#else

#ifndef PRId8
#define PRId8 "d"
#endif

#ifndef PRId16
#define PRId16 "d"
#endif

#ifndef PRIu32
#define PRIu32 "u"
#endif

#ifndef PRIu64
#if (ULONG_MAX == 4294967295UL)
#define PRIu64 "llu"
#else
#define PRIu64 "lu"
#endif /* (ULONG_MAX == 4294967295UL) */
#endif /* PRIu64 */
#endif /* HAVE_INTTYPES_H */
#endif /* HAVE_STDIO_H */

#if ((defined efloat32_exists) && (efloat32_exists))
#if HAVE_MEMCPY
efloat32 uint32_bits_to_efloat32(uint32_t u)
{
	efloat32 f;
	memcpy(&f, &u, sizeof(efloat32));
	return f;
}

uint32_t efloat32_to_uint32_bits(efloat32 f)
{
	uint32_t u;
	memcpy(&u, &f, sizeof(uint32_t));
	return u;
}

#if !SKIP_EFLOAT_SIGNED_INTS
efloat32 int32_bits_to_efloat32(int32_t i)
{
	efloat32 f;
	memcpy(&f, &i, sizeof(efloat32));
	return f;
}

int32_t efloat32_to_int32_bits(efloat32 f)
{
	int32_t i;
	memcpy(&i, &f, sizeof(int32_t));
	return i;
}
#endif /* !SKIP_EFLOAT_SIGNED_INTS */
#else /* HAVE_MEMCPY */

efloat32 uint32_bits_to_efloat32(uint32_t u)
{
	union efloat32_u {
		efloat32 f;
		uint32_t u;
	} pun;
	pun.u = u;
	return pun.f;
}

uint32_t efloat32_to_uint32_bits(efloat32 f)
{
	union efloat32_u {
		efloat32 f;
		uint32_t u;
	} pun;
	pun.f = f;
	return pun.u;
}

#if !SKIP_EFLOAT_SIGNED_INTS
efloat32 int32_bits_to_efloat32(int32_t i)
{
	union efloat32_u {
		efloat32 f;
		int32_t i;
	} pun;
	pun.i = i;
	return pun.f;
}

int32_t efloat32_to_int32_bits(efloat32 f)
{
	union efloat32_u {
		efloat32 f;
		int32_t i;
	} pun;
	pun.f = f;
	return pun.i;
}
#endif /* !SKIP_EFLOAT_SIGNED_INTS */
#endif /* HAVE_MEMCPY */

enum efloat_class efloat32_classify(efloat32 f)
{
	struct efloat32_fields fields;

	fields.sign = 0;
	fields.exponent = 0;
	fields.significand = 0;
	return efloat32_radix_2_to_fields(f, &fields);
}

enum efloat_class efloat32_radix_2_to_fields(efloat32 f,
					     struct efloat32_fields *fields)
{
	uint32_t u32, raw_significand;
	int32_t raw_exp;
	u32 = efloat32_to_uint32_bits(f);

	fields->sign = (u32 & efloat32_r2_sign_mask) ? -1 : 1;

	raw_exp = (u32 & efloat32_r2_rexp_mask);
	raw_exp = (raw_exp >> efloat32_r2_exp_shift);
	fields->exponent = (raw_exp - efloat32_r2_exp_max);

	raw_significand = (u32 & efloat32_r2_signif_mask);
	fields->significand = (fields->exponent == 0)
	    ? (raw_significand << 1)
	    : (raw_significand | (efloat32_r2_signif_mask + 1));

	if (fields->exponent == efloat32_r2_exp_inf_nan) {
		if (raw_significand) {
			/* +/- nan */
			return ef_nan;
		} else {
			/* +/- inf */
			return ef_inf;
		}
	}

	if ((raw_significand == 0) && (fields->exponent == efloat32_r2_exp_min)) {
		/* zero or -zero */
		return ef_zero;
	}

	if (fields->exponent == efloat32_r2_exp_min) {
		/* subnormal */
		return ef_subnorm;
	}

	return ef_normal;
}

efloat32 efloat32_radix_2_from_fields(const struct efloat32_fields fields,
				      enum efloat_class *efloat32class)
{
	efloat32 f;
	uint8_t raw_sign;
	int32_t raw_exp;
	uint32_t u32, raw_significand;
	struct efloat32_fields f2;
	int err;

	err = 0;
	raw_sign = fields.sign < 0 ? 1 : 0;

	raw_exp = fields.exponent;
	if (raw_exp > efloat32_r2_exp_inf_nan || raw_exp < efloat32_r2_exp_min) {
		seterrnoinval();
		err = 1;
		raw_exp = efloat32_r2_exp_inf_nan;
	}
	raw_exp = (raw_exp + efloat32_r2_exp_max);

	raw_significand = (fields.exponent == 0)
	    ? (fields.significand >> 1)
	    : (fields.significand);

	if (raw_significand != (raw_significand & efloat32_r2_signif_mask)) {
		seterrnoinval();
		err = 1;
		raw_significand = (raw_significand & efloat32_r2_signif_mask);
	}

	u32 = (raw_sign ? efloat32_r2_sign_mask : 0)
	    | (raw_exp << efloat32_r2_exp_shift)
	    | (raw_significand);

	f = uint32_bits_to_efloat32(u32);
	if (efloat32class) {
		*efloat32class = efloat32_radix_2_to_fields(f, &f2);
	} else {
		efloat32_radix_2_to_fields(f, &f2);
	}
	if (!err) {
		if ((!fields.sign) != (!f2.sign)) {
			seterrnoinval();
			eprintf2("sign %u != %u\n",
				 (unsigned)fields.sign, (unsigned)f2.sign);
		}
		if (fields.exponent != f2.exponent) {
			seterrnoinval();
			eprintf2("exponent %d != %d\n",
				 (int)fields.exponent, (int)f2.exponent);
		}
		if (fields.significand != f2.significand) {
			seterrnoinval();
			eprintf2("significand %lu != %lu\n",
				 (unsigned long)fields.significand,
				 (unsigned long)fields.significand);
		}
	}
	return f;
}

#if HAVE_STDIO_H
char *efloat32_fields_to_expression(const struct efloat32_fields fields,
				    char *buf, size_t len, int *written)
{
	const char *fmt;
	int rv;

	fmt = "(%" PRId8 " * (2^%" PRId16 ") * (%" PRIu32 " / (2^%u)))";
	rv = 0;

#if HAVE_SNPRINTF
	rv = snprintf(buf, len, fmt, fields.sign, fields.exponent,
		      fields.significand, efloat_float_exp_shift);
#else
	if (len <= (strlen(fmt) + 15)) {
		rv = -1;
	} else {
		rv = sprintf(buf, fmt, fields.sign, fields.exponent,
			     fields.significand, efloat_float_exp_shift);
	}
#endif /* HAVE_SNPRINTF */

	if (written) {
		*written = rv;
	}
	return (rv > 0) ? buf : NULL;
}
#endif /* HAVE_STDIO_H */

uint32_t efloat32_distance(efloat32 x, efloat32 y)
{
	uint32_t xu, yu;
	enum efloat_class x_fpclass, y_fpclass;
	struct efloat32_fields x_fields, y_fields;

	x_fields.sign = 0;
	x_fields.exponent = 0;
	x_fields.significand = 0;
	x_fpclass = efloat32_radix_2_to_fields(x, &x_fields);

	y_fields.sign = 0;
	y_fields.exponent = 0;
	y_fields.significand = 0;
	y_fpclass = efloat32_radix_2_to_fields(y, &y_fields);

	if (x_fpclass == ef_nan || y_fpclass == ef_nan) {
		return UINT32_MAX;
	}

	if (x_fpclass == ef_inf || y_fpclass == ef_inf) {
		if (x_fpclass == ef_inf && y_fpclass == ef_inf
		    && x_fields.sign == y_fields.sign) {
			return 0;
		}
		return UINT32_MAX;
	}

	if (x == y) {
		return 0;
	}

	xu = efloat32_to_uint32_bits(x_fields.sign < 0 ? -x : x);
	yu = efloat32_to_uint32_bits(y_fields.sign < 0 ? -y : y);

	if (x_fields.sign == y_fields.sign) {
		return xu < yu ? yu - xu : xu - yu;
	}

	return xu + yu;
}
#endif

#if ((defined efloat64_exists) && (efloat64_exists))
#if HAVE_MEMCPY
efloat64 uint64_bits_to_efloat64(uint64_t u)
{
	efloat64 f;
	memcpy(&f, &u, sizeof(efloat64));
	return f;
}

uint64_t efloat64_to_uint64_bits(efloat64 f)
{
	uint64_t u;
	memcpy(&u, &f, sizeof(uint64_t));
	return u;
}

#if !SKIP_EFLOAT_SIGNED_INTS
efloat64 int64_bits_to_efloat64(int64_t i)
{
	efloat64 f;
	memcpy(&f, &i, sizeof(efloat64));
	return f;
}

int64_t efloat64_to_int64_bits(efloat64 f)
{
	int64_t i;
	memcpy(&i, &f, sizeof(int64_t));
	return i;
}
#endif /* !SKIP_EFLOAT_SIGNED_INTS */
#else /* HAVE MEMCPY */
efloat64 uint64_bits_to_efloat64(uint64_t u)
{
	union efloat64_u {
		efloat64 f;
		uint64_t u;
	} pun;
	pun.u = u;
	return pun.f;
}

uint64_t efloat64_to_uint64_bits(efloat64 f)
{
	union efloat64_u {
		efloat64 f;
		uint64_t u;
	} pun;
	pun.f = f;
	return pun.u;
}

#if !SKIP_EFLOAT_SIGNED_INTS
efloat64 int64_bits_to_efloat64(int64_t i)
{
	union efloat64_u {
		efloat64 f;
		int64_t i;
	} pun;
	pun.i = i;
	return pun.f;
}

int64_t efloat64_to_int64_bits(efloat64 f)
{
	union efloat64_u {
		efloat64 f;
		int64_t i;
	} pun;
	pun.f = f;
	return pun.i;
}
#endif /* !SKIP_EFLOAT_SIGNED_INTS */
#endif /* HAVE_MEMCPY */

enum efloat_class efloat64_classify(efloat64 f)
{
	struct efloat64_fields fields;

	fields.sign = 0;
	fields.exponent = 0;
	fields.significand = 0;
	return efloat64_radix_2_to_fields(f, &fields);
}

enum efloat_class efloat64_radix_2_to_fields(efloat64 f,
					     struct efloat64_fields *fields)
{
	uint64_t u64, raw_exp, raw_significand;

	u64 = efloat64_to_uint64_bits(f);

	fields->sign = (u64 & efloat64_r2_sign_mask) ? -1 : 1;

	raw_exp = (u64 & efloat64_r2_rexp_mask);
	raw_exp = (raw_exp >> efloat64_r2_exp_shift);
	fields->exponent = (raw_exp - efloat64_r2_exp_max);

	raw_significand = u64 & efloat64_r2_signif_mask;
	fields->significand = (fields->exponent == 0)
	    ? raw_significand << 1
	    : raw_significand | (efloat64_r2_signif_mask + 1);

	if (fields->exponent == efloat64_r2_exp_inf_nan) {
		if (raw_significand) {
			/* +/- nan */
			return ef_nan;
		} else {
			/* +/- inf */
			return ef_inf;
		}
	}

	if ((raw_significand == 0)
	    && (fields->exponent == efloat64_r2_exp_min)) {
		/* zero or -zero */
		return ef_zero;
	}

	if (fields->exponent == efloat64_r2_exp_min) {
		/* subnormal */
		return ef_subnorm;
	}

	return ef_normal;
}

efloat64 efloat64_radix_2_from_fields(const struct efloat64_fields fields,
				      enum efloat_class *efloat64class)
{
	efloat64 f;
	uint8_t raw_sign;
	int64_t raw_exp;
	uint64_t u64, raw_significand;
	struct efloat64_fields f2;
	int err;

	err = 0;
	raw_sign = fields.sign < 0 ? 1 : 0;

	raw_exp = fields.exponent;
	if (raw_exp > efloat64_r2_exp_inf_nan || raw_exp < efloat64_r2_exp_min) {
		seterrnoinval();
		err = 1;
		raw_exp = efloat64_r2_exp_inf_nan;
	}
	raw_exp = (raw_exp + efloat64_r2_exp_max);

	raw_significand = (fields.exponent == 0)
	    ? (fields.significand >> 1)
	    : (fields.significand);

	if (raw_significand != (raw_significand & efloat64_r2_signif_mask)) {
		seterrnoinval();
		err = 1;
		raw_significand = (raw_significand & efloat64_r2_signif_mask);
	}

	u64 = (raw_sign ? efloat64_r2_sign_mask : 0)
	    | (raw_exp << efloat64_r2_exp_shift)
	    | (raw_significand);

	f = uint64_bits_to_efloat64(u64);
	if (efloat64class) {
		*efloat64class = efloat64_radix_2_to_fields(f, &f2);
	} else {
		efloat64_radix_2_to_fields(f, &f2);
	}
	if (!err) {
		if ((!fields.sign) != (!f2.sign)) {
			seterrnoinval();
			eprintf2("sign %u != %u\n",
				 (unsigned)fields.sign, (unsigned)f2.sign);
		}
		if (fields.exponent != f2.exponent) {
			seterrnoinval();
			eprintf2("exponent %d != %d\n",
				 (int)fields.exponent, (int)f2.exponent);
		}
		if (fields.significand != f2.significand) {
			seterrnoinval();
			eprintf2("significand %lu != %lu\n",
				 (unsigned long)fields.significand,
				 (unsigned long)fields.significand);
		}
	}
	return f;
}

#if HAVE_STDIO_H
char *efloat64_fields_to_expression(const struct efloat64_fields fields,
				    char *buf, size_t len, int *written)
{
	const char *fmt;
	int rv;

	fmt = "(%" PRId8 " * (2^%" PRId16 ") * (%" PRIu64 " / (2^%u)))";
	rv = 0;

#if HAVE_SNPRINTF
	rv = snprintf(buf, len, fmt, fields.sign, fields.exponent,
		      fields.significand, efloat_double_exp_shift);
#else
	if (len <= (strlen(fmt) + 30)) {
		rv = -1;
	} else {
		rv = sprintf(buf, fmt, fields.sign, fields.exponent,
			     fields.significand, efloat_double_exp_shift);
	}
#endif /* HAVE_SNPRINTF */

	if (written) {
		*written = rv;
	}

	return rv > 0 ? buf : NULL;
}
#endif /* HAVE_STDIO_H */

uint64_t efloat64_distance(efloat64 x, efloat64 y)
{
	uint64_t xu, yu;
	enum efloat_class x_fpclass, y_fpclass;
	struct efloat64_fields x_fields, y_fields;

	x_fields.sign = 0;
	x_fields.exponent = 0;
	x_fields.significand = 0;
	x_fpclass = efloat64_radix_2_to_fields(x, &x_fields);

	y_fields.sign = 0;
	y_fields.exponent = 0;
	y_fields.significand = 0;
	y_fpclass = efloat64_radix_2_to_fields(y, &y_fields);

	if (x_fpclass == ef_nan || y_fpclass == ef_nan) {
		return UINT64_MAX;
	}

	if (x_fpclass == ef_inf || y_fpclass == ef_inf) {
		if (x_fpclass == ef_inf && y_fpclass == ef_inf
		    && x_fields.sign == y_fields.sign) {
			return 0;
		}
		return UINT64_MAX;
	}

	if (x == y) {
		return 0;
	}

	xu = efloat64_to_uint64_bits(x_fields.sign < 0 ? -x : x);
	yu = efloat64_to_uint64_bits(y_fields.sign < 0 ? -y : y);

	if (x_fields.sign == y_fields.sign) {
		return xu < yu ? yu - xu : xu - yu;
	}

	return xu + yu;
}
#endif
