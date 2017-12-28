/*
efloat.c: Embedable Float manipulation library
Copyright (C) 2017 Eric Herman

https://github.com/ericherman/libefloat

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later
version.
*/

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
#if (HAVE_MEMCPY)
#include <string.h>		/* memcpy */
#endif
/*
   If no memcpy(), then we can use type-punning via a union.
   The C standard reads ambuguously to me as to whether this is strictly
   conforming, but I know of no compilers which do not support it.

   The calls to memcpy() here are probably going to be removed by gcc:
   gcc output: https://godbolt.org/g/hiX8oL
*/

#if ((defined efloat32_exists) && (efloat32_exists))
#if HAVE_MEMCPY
efloat32 uint32_to_efloat32(uint32_t u)
{
	efloat32 f;
	memcpy(&f, &u, sizeof(efloat32));
	return f;
}

uint32_t efloat32_to_uint32(efloat32 f)
{
	uint32_t u;
	memcpy(&u, &f, sizeof(uint32_t));
	return u;
}

#if ((defined efloat32_also_signed_ints) && (efloat32_also_signed_ints))
efloat32 int32_to_efloat32(int32_t i)
{
	efloat32 f;
	memcpy(&f, &i, sizeof(efloat32));
	return f;
}

int32_t efloat32_to_int32(efloat32 f)
{
	int32_t i;
	memcpy(&i, &f, sizeof(int32_t));
	return i;
}
#endif /* efloat32_also_signed_ints */
#else /* HAVE_MEMCPY */

efloat32 uint32_to_efloat32(uint32_t u)
{
	union efloat32_u {
		efloat32 f;
		uint32_t u;
	} pun;
	pun.u = u;
	return pun.f;
}

uint32_t efloat32_to_uint32(efloat32 f)
{
	union efloat32_u {
		efloat32 f;
		uint32_t u;
	} pun;
	pun.f = f;
	return pun.u;
}

#if ((defined efloat64_also_signed_ints) && (efloat64_also_signed_ints))
efloat32 int32_to_efloat32(int32_t i)
{
	union efloat32_u {
		efloat32 f;
		int32_t i;
	} pun;
	pun.i = i;
	return pun.f;
}

int32_t efloat32_to_int32(efloat32 f)
{
	union efloat32_u {
		efloat32 f;
		int32_t i;
	} pun;
	pun.f = f;
	return pun.i;
}
#endif /* efloat64_also_signed_ints */
#endif /* HAVE_MEMCPY */

enum efloat_class efloat32_classify(efloat32 f)
{
	uint8_t sign;
	int16_t exp;
	uint32_t signif;

	sign = 0;
	exp = 0;
	signif = 0;
	return efloat32_radix_2_to_fields(f, &sign, &exp, &signif);
}

enum efloat_class efloat32_radix_2_to_fields(efloat32 f,
					     uint8_t *sign,
					     int16_t *exponent,
					     uint32_t *significand)
{
	uint32_t u32, raw_exp;

	u32 = efloat32_to_uint32(f);

	*sign = (u32 & efloat32_r2_sign_mask) ? 1U : 0U;

	raw_exp = (u32 & efloat32_r2_rexp_mask);
	raw_exp = (raw_exp >> efloat32_r2_exp_shift);
	*exponent = (raw_exp - efloat32_r2_exp_max);

	*significand = u32 & efloat32_r2_signif_mask;

	if (*exponent == efloat32_r2_exp_inf_nan) {
		if (*significand) {
			/* +/- nan */
			return ef_nan;
		} else {
			/* +/- inf */
			return ef_inf;
		}
	}

	if (*significand == 0 && *exponent == efloat32_r2_exp_min) {
		/* zero or -zero */
		return ef_zero;
	}

	if (*exponent == efloat32_r2_exp_min) {
		/* subnormal */
		return ef_subnorm;
	}

	return ef_normal;
}

efloat32 efloat32_radix_2_from_fields(uint8_t sign,
				      int16_t exponent,
				      uint32_t significand,
				      enum efloat_class *efloat32class)
{
	efloat32 f;
	uint32_t u32, raw_exp;
	int16_t exp2;
	uint8_t s2;
	uint32_t signif2;
	int err;

	err = 0;
	s2 = sign ? 1 : 0;

	if (exponent > efloat32_r2_exp_inf_nan
	    || exponent < efloat32_r2_exp_min) {
		err = 1;
		exponent = efloat32_r2_exp_inf_nan;
	}
	raw_exp = (exponent + efloat32_r2_exp_max);

	if (significand != (significand & efloat32_r2_signif_mask)) {
		err = 1;
		significand = (significand & efloat32_r2_signif_mask);
	}

	u32 = (sign ? efloat32_r2_sign_mask : 0)
	    | (raw_exp << efloat32_r2_exp_shift)
	    | significand;

	f = uint32_to_efloat32(u32);
	if (efloat32class) {
		*efloat32class =
		    efloat32_radix_2_to_fields(f, &s2, &exp2, &signif2);
	} else {
		efloat32_radix_2_to_fields(f, &s2, &exp2, &signif2);
	}
	if (!err) {
		if ((!sign) != (!s2)) {
			eprintf2("sign %u != %u\n", (unsigned)sign,
				 (unsigned)s2);
		}
		if (exponent != exp2) {
			eprintf2("exponent %d != %d\n", (int)exponent,
				 (int)exp2);
		}
		if (significand != signif2) {
			eprintf2("significand %lu != %lu\n",
				 (unsigned long)significand,
				 (unsigned long)signif2);
		}
	}
	return f;
}
#endif

#if ((defined efloat64_exists) && (efloat64_exists))
#if HAVE_MEMCPY
efloat64 uint64_to_efloat64(uint64_t u)
{
	efloat64 f;
	memcpy(&f, &u, sizeof(efloat64));
	return f;
}

uint64_t efloat64_to_uint64(efloat64 f)
{
	uint64_t u;
	memcpy(&u, &f, sizeof(uint64_t));
	return u;
}

#if ((defined efloat64_also_signed_ints) && (efloat64_also_signed_ints))
efloat64 int64_to_efloat64(int64_t i)
{
	efloat64 f;
	memcpy(&f, &i, sizeof(efloat64));
	return f;
}

int64_t efloat64_to_int64(efloat64 f)
{
	int64_t i;
	memcpy(&i, &f, sizeof(int64_t));
	return i;
}
#endif /* efloat64_also_signed_ints */
#else /* HAVE MEMCPY */
efloat64 uint64_to_efloat64(uint64_t u)
{
	union efloat64_u {
		efloat64 f;
		uint64_t u;
	} pun;
	pun.u = u;
	return pun.f;
}

uint64_t efloat64_to_uint64(efloat64 f)
{
	union efloat64_u {
		efloat64 f;
		uint64_t u;
	} pun;
	pun.f = f;
	return pun.u;
}

#if ((defined efloat64_also_signed_ints) && (efloat64_also_signed_ints))
efloat64 int64_to_efloat64(int64_t i)
{
	union efloat64_u {
		efloat64 f;
		int64_t i;
	} pun;
	pun.i = i;
	return pun.f;
}

int64_t efloat64_to_int64(efloat64 f)
{
	union efloat64_u {
		efloat64 f;
		int64_t i;
	} pun;
	pun.f = f;
	return pun.i;
}
#endif /* efloat64_also_signed_ints */
#endif /* HAVE_MEMCPY */

enum efloat_class efloat64_classify(efloat64 f)
{
	uint8_t sign;
	int16_t exp;
	uint64_t signif;

	sign = 0;
	exp = 0;
	signif = 0;
	return efloat64_radix_2_to_fields(f, &sign, &exp, &signif);
}

enum efloat_class efloat64_radix_2_to_fields(efloat64 f,
					     uint8_t *sign,
					     int16_t *exponent,
					     uint64_t *significand)
{
	uint64_t u64, raw_exp;

	u64 = efloat64_to_uint64(f);

	*sign = (u64 & efloat64_r2_sign_mask) ? 1U : 0U;

	raw_exp = (u64 & efloat64_r2_rexp_mask);
	raw_exp = (raw_exp >> efloat64_r2_exp_shift);
	*exponent = (raw_exp - efloat64_r2_exp_max);

	*significand = u64 & efloat64_r2_signif_mask;

	if (*exponent == efloat64_r2_exp_inf_nan) {
		if (*significand) {
			/* +/- nan */
			return ef_nan;
		} else {
			/* +/- inf */
			return ef_inf;
		}
	}

	if (*significand == 0 && *exponent == efloat64_r2_exp_min) {
		/* zero or -zero */
		return ef_zero;
	}

	if (*exponent == efloat64_r2_exp_min) {
		/* subnormal */
		return ef_subnorm;
	}

	return ef_normal;
}

efloat64 efloat64_radix_2_from_fields(uint8_t sign,
				      int16_t exponent,
				      uint64_t significand,
				      enum efloat_class *efloat64class)
{
	efloat64 f;
	uint64_t u64, raw_exp;
	int16_t exp2;
	uint8_t s2;
	uint64_t signif2;
	int err;

	err = 0;
	s2 = sign ? 1 : 0;

	if (exponent > efloat64_r2_exp_inf_nan
	    || exponent < efloat64_r2_exp_min) {
		err = 1;
		exponent = efloat64_r2_exp_inf_nan;
	}
	raw_exp = (exponent + efloat64_r2_exp_max);

	if (significand != (significand & efloat64_r2_signif_mask)) {
		err = 1;
		significand = (significand & efloat64_r2_signif_mask);
	}

	u64 = (sign ? efloat64_r2_sign_mask : 0)
	    | (raw_exp << efloat64_r2_exp_shift)
	    | significand;

	f = uint64_to_efloat64(u64);
	if (efloat64class) {
		*efloat64class =
		    efloat64_radix_2_to_fields(f, &s2, &exp2, &signif2);
	} else {
		efloat64_radix_2_to_fields(f, &s2, &exp2, &signif2);
	}
	if (!err) {
		if ((!sign) != (!s2)) {
			eprintf2("sign %u != %u\n", (unsigned)sign,
				 (unsigned)s2);
		}
		if (exponent != exp2) {
			eprintf2("exponent %d != %d\n", (int)exponent,
				 (int)exp2);
		}
		if (significand != signif2) {
			eprintf2("significand %lu != %lu\n",
				 (unsigned long)significand,
				 (unsigned long)signif2);
		}
	}
	return f;
}
#endif
