/*
efloat.h: Embedable Float manipulation library
Copyright (C) 2017 Eric Herman

https://github.com/ericherman/libefloat

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later
version.
*/

/*
Assuming IEEE 754 (radix 2), transforming the exponent is a bit tricky, but
the bit layout is straight-forward:

float32 Sign	Exponent	Significand (mantissa)
float32 1 [31]	8 [30-23]	23 [22-00]
float32 SEEEEEEE EMMMMMMM MMMMMMMM MMMMMMMM
https://en.wikipedia.org/wiki/Single-precision_floating-point_format

float64 Sign	Exponent	Significand (mantissa)
float64 1 [63]	11 [62-52]	52 [51-00]
float64 SEEEEEEE EEEEMMMM MMMMMMMM MMMMMMMM MMMMMMMM MMMMMMMM MMMMMMMM MMMMMMMM
https://en.wikipedia.org/wiki/Double-precision_floating-point_format
*/

#ifndef EFLOAT_H
#define EFLOAT_H

#ifdef __cplusplus
extern "C" {
#endif

#if HAVE_STDINT_H
#include <stdint.h>
#endif

#if HAVE_LIMITS_H
#include <limits.h>
#endif

#if HAVE_FLOAT_H
#include <float.h>
#endif

#if ((!(defined efloat_float)) \
 && (FLT_MAX_EXP == 1024) \
 && (FLT_MANT_DIG == 53) \
 && (FLT_RADIX == 2))
#define efloat_float 64
#endif

#if ((!(defined efloat_float)) \
 && (FLT_MAX_EXP == 128) \
 && (FLT_MANT_DIG == 24) \
 && (FLT_RADIX == 2))
#define efloat_float 32
#endif

#if ((!(defined efloat_double)) \
 && (DBL_MAX_EXP == 1024) \
 && (DBL_MANT_DIG == 53) \
 && (FLT_RADIX == 2))
#define efloat_double 64
#endif

#if ((!(defined efloat_double)) \
 && (DBL_MAX_EXP == 128) \
 && (DBL_MANT_DIG == 24) \
 && (FLT_RADIX == 2))
#define efloat_double 32
#endif

#if ((!(defined efloat_long_double)) \
 && (LDBL_MAX_EXP == 1024) \
 && (LDBL_MANT_DIG == 53) \
 && (FLT_RADIX == 2))
#define efloat_long_double 64
#endif

#if ((!(defined efloat_long_double)) \
 && (LDBL_MAX_EXP == 128) \
 && (LDBL_MANT_DIG == 24) \
 && (FLT_RADIX == 2))
#define efloat_long_double 32
#endif

#if (((!(defined efloat64_exists)) || (efloat64_exists == 0)) \
 && ((defined efloat_double) && (efloat_double == 64)))
#define efloat64_exists 1
typedef double efloat64;
#endif

#if (((!(defined efloat64_exists)) || (efloat64_exists == 0)) \
 && ((defined efloat_long_double) && (efloat_long_double == 64)))
#define efloat64_exists 1
typedef long double efloat64;
#endif

#if (((!(defined efloat64_exists)) || (efloat64_exists == 0)) \
 && ((defined efloat_float) && (efloat_float == 64)))
#define efloat64_exists 1
typedef float efloat64;
#endif

#if (((!(defined efloat32_exists)) || (efloat32_exists == 0)) \
 && ((defined efloat_double) && (efloat_double == 32)))
#define efloat32_exists 1
typedef double efloat32;
#endif

#if (((!(defined efloat32_exists)) || (efloat32_exists == 0)) \
 && ((defined efloat_float) && (efloat_float == 32)))
#define efloat32_exists 1
typedef float efloat32;
#endif

#if (((!(defined efloat32_exists)) || (efloat32_exists == 0)) \
 && ((defined efloat_long_double) && (efloat_long_double == 32)))
#define efloat32_exists 1
typedef long double efloat32;
#endif

enum efloat_class {
	ef_nan = 0,
	ef_inf = 1,
	ef_zero = 2,
	ef_subnorm = 3,
	ef_normal = 4
};

#if ((defined efloat32_exists) && (efloat32_exists))
#define efloat32_r2_exp_max 127
#define efloat32_r2_exp_min -127
#define efloat32_r2_exp_inf_nan 128
#define efloat32_r2_sign_mask 0x80000000UL
#define efloat32_r2_rexp_mask 0x7F800000UL
#define efloat32_r2_signif_mask 0x007FFFFFUL
#define efloat32_r2_exp_shift 23
#endif

#if ((defined efloat64_exists) && (efloat64_exists) \
 && (__SIZEOF_LONG__ >= 8))
#define efloat64_r2_exp_max 1023L
#define efloat64_r2_exp_min -1023L
#define efloat64_r2_exp_inf_nan 1024L
#define efloat64_r2_sign_mask 0x8000000000000000UL
#define efloat64_r2_rexp_mask 0x7FF0000000000000UL
#define efloat64_r2_signif_mask 0x000FFFFFFFFFFFFFUL
#define efloat64_r2_exp_shift 52
#endif

#if ((defined efloat64_exists) && (efloat64_exists) \
 && (__SIZEOF_LONG__ < 8 &&  __SIZEOF_LONG_LONG__ >= 8))
#define efloat64_r2_exp_max 1023L
#define efloat64_r2_exp_min -1023L
#define efloat64_r2_exp_inf_nan 1024L
#define efloat64_r2_sign_mask 0x8000000000000000ULL
#define efloat64_r2_rexp_mask 0x7FF0000000000000ULL
#define efloat64_r2_signif_mask 0x000FFFFFFFFFFFFFULL
#define efloat64_r2_exp_shift 52
#endif

#if ((defined efloat32_exists) && (efloat32_exists))
efloat32 int32_to_efloat32(int32_t i);
efloat32 uint32_to_efloat32(uint32_t i);
uint32_t efloat32_to_uint32(efloat32 f);
int32_t efloat32_to_int32(efloat32 f);
enum efloat_class efloat32_classify(efloat32 f);
enum efloat_class efloat32_radix_2_to_fields(efloat32 f,
					     uint8_t *sign,
					     int16_t *exponent,
					     uint32_t *significand);
efloat32 efloat32_radix_2_from_fields(uint8_t sign,
				      int16_t exponent,
				      uint32_t significand,
				      enum efloat_class *efloat32class);
#endif

#if ((defined efloat64_exists) && (efloat64_exists))
efloat64 int64_to_efloat64(int64_t i);
efloat64 uint64_to_efloat64(uint64_t i);
uint64_t efloat64_to_uint64(efloat64 f);
int64_t efloat64_to_int64(efloat64 f);
enum efloat_class efloat64_classify(efloat64 f);
enum efloat_class efloat64_radix_2_to_fields(efloat64 f,
					     uint8_t *sign,
					     int16_t *exponent,
					     uint64_t *significand);
efloat64 efloat64_radix_2_from_fields(uint8_t sign,
				      int16_t exponent,
				      uint64_t significand,
				      enum efloat_class *efloat64class);
#endif

#ifdef __cplusplus
}
#endif

#endif /* EFLOAT_H */
