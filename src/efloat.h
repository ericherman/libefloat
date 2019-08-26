/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* efloat.h: Embedable Float manipulation library */
/* Copyright (C) 2017, 2018, 2019 Eric Herman */
/* https://github.com/ericherman/libefloat */

/*
Assuming IEEE 754 (radix 2), transforming the exponent is a bit tricky, but
the bit layout is straight-forward:

float32 Sign	Exponent	Significand (mantissa)
float32 1 [31]	8 [30-23]	23 [22-00]
float32 SEEEEEEE EMMMMMMM MMMMMMMM MMMMMMMM
https://en.wikipedia.org/wiki/Single-precision_floating-point_format

	int sign, exponent;
	uint32_t significand;

	sign = ((bits >> 31) == 0) ? 1 : -1;
	exponent = ((bits >> 23) & 0xff);
	significand = (exponent == 0)
                    ? (bits & 0x7fffff) << 1
                    : (bits & 0x7fffff) | 0x800000;

float64 Sign	Exponent	Significand (mantissa)
float64 1 [63]	11 [62-52]	52 [51-00]
float64 SEEEEEEE EEEEMMMM MMMMMMMM MMMMMMMM MMMMMMMM MMMMMMMM MMMMMMMM MMMMMMMM
https://en.wikipedia.org/wiki/Double-precision_floating-point_format

	int sign, exponent;
	uint64_t significand;

	sign = ((bits >> 63) == 0) ? 1 : -1;
	exponent = (int)((bits >> 52) & 0x7ffL);
	significand = (exponent == 0)
                    ? (bits & 0xfffffffffffffL) << 1
                    : (bits & 0xfffffffffffffL) | 0x10000000000000L;

See also:
https://en.wikipedia.org/wiki/IEEE_754-1985
https://en.wikipedia.org/wiki/IEEE_754-2008

https://docs.oracle.com/javase/7/docs/api/java/lang/Float.html#floatToIntBits(float)
https://docs.oracle.com/javase/7/docs/api/java/lang/Float.html#intBitsToFloat(int)
https://docs.oracle.com/javase/7/docs/api/java/lang/Double.html#DoubleToLongBits(double)
https://docs.oracle.com/javase/7/docs/api/java/lang/Double.html#longBitsToDouble(long)

*/

#ifndef EFLOAT_H
#define EFLOAT_H

#ifdef __cplusplus
#define Efloat_begin_C_functions extern "C" {
#define Efloat_end_C_functions }
#else
#define Efloat_begin_C_functions
#define Efloat_end_C_functions
#endif

#if HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_STDDEF_H
#include <stddef.h>
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

#if HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#ifndef SKIP_EFLOAT_SIGNED_INTS
#define SKIP_EFLOAT_SIGNED_INTS 0
#endif

/* maybe one day we will want RADIX != 2, but I doubt it */
#define efloat32_radix 2

#if ((!(defined efloat_float)) \
 && (FLT_MAX_EXP == 128) \
 && (FLT_MANT_DIG == 24) \
 && (FLT_RADIX == 2))
#define efloat_float 32
#endif

#if ((!(defined efloat_float)) \
 && (FLT_MAX_EXP == 1024) \
 && (FLT_MANT_DIG == 53) \
 && (FLT_RADIX == 2))
#define efloat_float 64
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
#define efloat64_max DBL_MAX
#define efloat64_min DBL_MIN
#define nextafter64(x,y) nextafter(x,y)
#define nexttoward64(x,y) nexttoward(x,y)
#endif

#if (((!(defined efloat64_exists)) || (efloat64_exists == 0)) \
 && ((defined efloat_long_double) && (efloat_long_double == 64)))
#define efloat64_exists 1
typedef long double efloat64;
#define efloat64_max LDBL_MAX
#define efloat64_min LDBL_MIN
#define nextafter32(x,y) nextafterl(x,y)
#define nexttoward32(x,y) nexttowardl(x,y)
#endif

#if (((!(defined efloat64_exists)) || (efloat64_exists == 0)) \
 && ((defined efloat_float) && (efloat_float == 64)))
#define efloat64_exists 1
typedef float efloat64;
#define efloat64_max FLT_MAX
#define efloat64_min FLT_MIN
#define nextafter64(x,y) nextafterf(x,y)
#define nexttoward64(x,y) nexttowardf(x,y)
#endif

#if (((!(defined efloat32_exists)) || (efloat32_exists == 0)) \
 && ((defined efloat_float) && (efloat_float == 32)))
#define efloat32_exists 1
typedef float efloat32;
#define efloat32_max FLT_MAX
#define efloat32_min FLT_MIN
#define nextafter32(x,y) nextafterf(x,y)
#define nexttoward32(x,y) nexttowardf(x,y)
#endif

#if (((!(defined efloat32_exists)) || (efloat32_exists == 0)) \
 && ((defined efloat_double) && (efloat_double == 32)))
#define efloat32_exists 1
typedef double efloat32;
#define efloat32_max DBL_MAX
#define efloat32_min DBL_MIN
#define nextafter32(x,y) nextafter(x,y)
#define nexttoward32(x,y) nexttoward(x,y)
#endif

#if (((!(defined efloat32_exists)) || (efloat32_exists == 0)) \
 && ((defined efloat_long_double) && (efloat_long_double == 32)))
#define efloat32_exists 1
typedef long double efloat32;
#define efloat32_max LDBL_MAX
#define efloat32_min LDBL_MIN
#define nextafter32(x,y) nextafterl(x,y)
#define nexttoward32(x,y) nexttowardl(x,y)
#endif

enum efloat_class {
	ef_nan = 0,
	ef_inf = 1,
	ef_zero = 2,
	ef_subnorm = 3,
	ef_normal = 4
};

struct efloat32_fields {
	int8_t sign;
	int16_t exponent;
	uint32_t significand;
};

struct efloat64_fields {
	int8_t sign;
	int16_t exponent;
	uint64_t significand;
};

#if ((defined efloat32_exists) && (efloat32_exists))
#define efloat32_r2_exp_max 127
#define efloat32_exp_max efloat32_r2_exp_max
#define efloat32_r2_exp_min -127
#define efloat32_exp_min efloat32_r2_exp_min
#define efloat32_mant_dig 24
#define efloat32_r2_exp_inf_nan 128
#define efloat32_exp_inf_nan efloat32_r2_exp_inf_nan
#define efloat32_r2_sign_mask 0x80000000UL
#define efloat32_r2_rexp_mask 0x7F800000UL
#define efloat32_r2_signif_mask 0x007FFFFFUL
#define efloat32_r2_exp_bits 8
#define efloat32_r2_exp_shift 23
#endif

#if ((defined efloat64_exists) && (efloat64_exists) \
 && (__SIZEOF_LONG__ >= 8))
#define efloat64_r2_exp_max 1023L
#define efloat64_exp_max efloat64_r2_exp_max
#define efloat64_r2_exp_min -1023L
#define efloat64_exp_min efloat64_r2_exp_min
#define efloat64_mant_dig 53
#define efloat64_r2_exp_inf_nan 1024L
#define efloat64_exp_inf_nan efloat64_r2_exp_inf_nan
#define efloat64_r2_sign_mask 0x8000000000000000UL
#define efloat64_r2_rexp_mask 0x7FF0000000000000UL
#define efloat64_r2_signif_mask 0x000FFFFFFFFFFFFFUL
#define efloat64_r2_exp_bits 11
#define efloat64_r2_exp_shift 52
#endif

Efloat_begin_C_functions
/* first the efloat32 functions */
#if efloat32_exists
efloat32 uint32_bits_to_efloat32(uint32_t i);
uint32_t efloat32_to_uint32_bits(efloat32 f);
#if (!SKIP_EFLOAT_SIGNED_INTS)
efloat32 int32_bits_to_efloat32(int32_t i);
int32_t efloat32_to_int32_bits(efloat32 f);
#endif /* (!SKIP_EFLOAT_SIGNED_INTS) */
enum efloat_class efloat32_classify(efloat32 f);
enum efloat_class efloat32_radix_2_to_fields(efloat32 f,
					     struct efloat32_fields *fields);
efloat32 efloat32_radix_2_from_fields(const struct efloat32_fields fields,
				      enum efloat_class *efloat32class);
#if HAVE_STDIO_H
char *efloat32_fields_to_expression(const struct efloat32_fields fields,
				    char *buf, size_t len, int *written);
#endif /* HAVE_STDIO_H */
uint32_t efloat32_distance(efloat32 x, efloat32 y);
#endif /* efloat32_exists */

/* next the efloat64 functions */

#if efloat64_exists
efloat64 uint64_bits_to_efloat64(uint64_t i);
uint64_t efloat64_to_uint64_bits(efloat64 f);
#if (!SKIP_EFLOAT_SIGNED_INTS)
efloat64 int64_bits_to_efloat64(int64_t i);
int64_t efloat64_to_int64_bits(efloat64 f);
#endif /* (!SKIP_EFLOAT_SIGNED_INTS) */
enum efloat_class efloat64_classify(efloat64 f);
enum efloat_class efloat64_radix_2_to_fields(efloat64 f,
					     struct efloat64_fields *fields);
efloat64 efloat64_radix_2_from_fields(const struct efloat64_fields fields,
				      enum efloat_class *efloat64class);
#if HAVE_STDIO_H
char *efloat64_fields_to_expression(const struct efloat64_fields fields,
				    char *buf, size_t len, int *written);
#endif /* HAVE_STDIO_H */
uint64_t efloat64_distance(efloat64 x, efloat64 y);
#endif /* efloat64_exists */

/* last the function aliases */

#if (efloat_float == 32)
#define efloat_float_exp_bits efloat32_r2_exp_bits
#define efloat_float_exp_shift efloat32_r2_exp_shift
#define efloat_float_fields efloat32_fields
#define efloat_float_to_fields(f,fields) efloat32_radix_2_to_fields(f,fields)
#define efloat_float_from_fields(fields, cls) \
	efloat32_radix_2_from_fields(fields, cls)
#define efloat_float_fields_to_expression(fields, buf, len, written) \
	efloat32_fields_to_expression(fields, buf, len, written)
#define efloat_float_exp_inf_nan efloat32_r2_exp_inf_nan
#endif

#if (efloat_float == 64)
#define efloat_float_exp_bits efloat64_r2_exp_bits
#define efloat_float_exp_shift efloat54_r2_exp_shift
#define efloat_float_fields efloat64_fields
#define efloat_float_to_fields(f,fields) efloat64_radix_2_to_fields(f,fields)
#define efloat_float_from_fields(fields, cls) \
	efloat64_radix_2_from_fields(fields, cls)
#define efloat_float_fields_to_expression(fields, buf, len, written) \
	efloat64_fields_to_expression(fields, buf, len, writeen)
#define efloat_float_exp_inf_nan efloat64_r2_exp_inf_nan
#endif

#if (efloat_double == 32)
#define efloat_double_exp_bits efloat32_r2_exp_bits
#define efloat_double_exp_shift efloat32_r2_exp_shift
#define efloat_double_fields efloat32_fields
#define efloat_double_to_fields(d,fields) efloat32_radix_2_to_fields(d,fields)
#define efloat_double_from_fields(fields, cls) \
	efloat32_radix_2_from_fields(fields, cls)
#define efloat_double_fields_to_expression(fields, buf, len, written) \
	efloat32_fields_to_expression(fields, buf, len, written)
#define efloat_double_exp_inf_nan efloat32_r2_exp_inf_nan
#endif

#if (efloat_double == 64)
#define efloat_double_exp_bits efloat64_r2_exp_bits
#define efloat_double_exp_shift efloat64_r2_exp_shift
#define efloat_double_fields efloat64_fields
#define efloat_double_to_fields(d,fields) efloat64_radix_2_to_fields(d,fields)
#define efloat_double_from_fields(fields, cls) \
	efloat64_radix_2_from_fields(fields, cls)
#define efloat_double_fields_to_expression(fields, buf, len, written) \
	efloat64_fields_to_expression(fields, buf, len, written)
#define efloat_double_exp_inf_nan efloat64_r2_exp_inf_nan
#endif

Efloat_end_C_functions
#undef Efloat_begin_C_functions
#undef Efloat_end_C_functions
#endif /* EFLOAT_H */
