/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* efloat.c: Embeddable Float manipulation library */
/* Copyright (C) 2017, 2018, 2019 Eric Herman */
/* https://github.com/ericherman/libefloat */

#include "efloat.h"
#include "eembed.h"

#ifndef UINT32_MAX
#define UINT32_MAX  (0xFFFFFFFF)
#endif

#ifndef UINT64_MAX
#define UINT64_MAX  (0xFFFFFFFFFFFFFFFF)
#endif

#if EEMBED_HOSTED
#include <errno.h>
void efloat_set_errno_einval(void)
{
	errno = EINVAL;
}

void (*efloat_seterrinval)(void) = efloat_set_errno_einval;
#else
void (*efloat_seterrinval)(void) = NULL;
#endif

#define Efloat_set_err_inval() \
	do { \
		if (efloat_seterrinval) { \
			efloat_seterrinval(); \
		} \
	} while (0)

#define Efloat_debug_print_str(str) do { \
	if (eembed_err_log) { \
		eembed_err_log->append_s(eembed_err_log, str); \
	} } while (0)

#define Efloat_debug_print_u64(u64) do { \
	if (eembed_err_log) { \
		eembed_err_log->append_ul(eembed_err_log, u64); \
	} } while (0)

#define Efloat_debug_print_i32(i32) do { \
	if (eembed_err_log) { \
		eembed_err_log->append_l(eembed_err_log, i32); \
	} } while (0)

#define Efloat_debug_print_eol() do { \
	if (eembed_err_log) { \
		eembed_err_log->append_eol(eembed_err_log); \
	} } while (0)

#if ((defined efloat32_exists) && (efloat32_exists))
static int32_t efloat32_to_int32_bits_memcpy(efloat32 f)
{
	int32_t i;
	eembed_memcpy(&i, &f, sizeof(int32_t));
	return i;
}

static int32_t efloat32_to_int32_bits_unionp(efloat32 f)
{
	union efloat32_u {
		efloat32 f;
		int32_t i;
	} pun;
	pun.f = f;
	return pun.i;
}

int32_t efloat32_to_int32_bits(efloat32 f)
{
	if (eembed_memcpy) {
		return efloat32_to_int32_bits_memcpy(f);
	} else {
		return efloat32_to_int32_bits_unionp(f);
	}
}

static uint32_t efloat32_to_uint32_bits_memcpy(efloat32 f)
{
	uint32_t u;
	eembed_memcpy(&u, &f, sizeof(uint32_t));
	return u;
}

static uint32_t efloat32_to_uint32_bits_unionp(efloat32 f)
{
	union efloat32_u {
		efloat32 f;
		uint32_t u;
	} pun;
	pun.f = f;
	return pun.u;
}

uint32_t efloat32_to_uint32_bits(efloat32 f)
{
	if (eembed_memcpy) {
		return efloat32_to_uint32_bits_memcpy(f);
	} else {
		return efloat32_to_uint32_bits_unionp(f);
	}
}

static efloat32 int32_bits_to_efloat32_memcpy(int32_t i)
{
	efloat32 f;
	eembed_memcpy(&f, &i, sizeof(efloat32));
	return f;
}

static efloat32 int32_bits_to_efloat32_unionp(int32_t i)
{
	union efloat32_u {
		efloat32 f;
		int32_t i;
	} pun;
	pun.i = i;
	return pun.f;
}

efloat32 int32_bits_to_efloat32(int32_t i)
{
	if (eembed_memcpy) {
		return int32_bits_to_efloat32_memcpy(i);
	} else {
		return int32_bits_to_efloat32_unionp(i);
	}
}

static efloat32 uint32_bits_to_efloat32_memcpy(uint32_t u)
{
	efloat32 f;
	eembed_memcpy(&f, &u, sizeof(efloat32));
	return f;
}

static efloat32 uint32_bits_to_efloat32_unionp(uint32_t u)
{
	union efloat32_u {
		efloat32 f;
		uint32_t u;
	} pun;
	pun.u = u;
	return pun.f;
}

efloat32 uint32_bits_to_efloat32(uint32_t u)
{
	if (eembed_memcpy) {
		return uint32_bits_to_efloat32_memcpy(u);
	} else {
		return uint32_bits_to_efloat32_unionp(u);
	}
}

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
		Efloat_set_err_inval();
		err = 1;
		raw_exp = efloat32_r2_exp_inf_nan;
	}
	raw_exp = (raw_exp + efloat32_r2_exp_max);

	raw_significand = (fields.exponent == 0)
	    ? (fields.significand >> 1)
	    : (fields.significand);

	if (raw_significand != (raw_significand & efloat32_r2_signif_mask)) {
		Efloat_set_err_inval();
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
			Efloat_set_err_inval();
			Efloat_debug_print_str("sign ");
			Efloat_debug_print_i32(fields.sign);
			Efloat_debug_print_str(" != ");
			Efloat_debug_print_i32(f2.sign);
			Efloat_debug_print_eol();
		}
		if (fields.exponent != f2.exponent) {
			Efloat_set_err_inval();
			Efloat_debug_print_str("exponent ");
			Efloat_debug_print_i32(fields.exponent);
			Efloat_debug_print_str(" != ");
			Efloat_debug_print_i32(f2.exponent);
			Efloat_debug_print_eol();
		}
		if (fields.significand != f2.significand) {
			Efloat_set_err_inval();
			Efloat_set_err_inval();
			Efloat_debug_print_str("exponent ");
			Efloat_debug_print_u64(fields.significand);
			Efloat_debug_print_str(" != ");
			Efloat_debug_print_u64(f2.significand);
			Efloat_debug_print_eol();
		}
	}
	return f;
}

char *efloat32_fields_to_expression(const struct efloat32_fields fields,
				    char *buf, size_t len, int *written)
{
	struct eembed_log log;
	struct eembed_str_buf sbuf;
	struct eembed_log *bl;

	if (buf && len) {
		buf[0] = '\0';
	}
	bl = eembed_char_buf_log_init(&log, &sbuf, buf, len);
	if (!bl) {
		return NULL;
	}

	bl->append_s(bl, "(");
	bl->append_l(bl, fields.sign);
	bl->append_s(bl, " * (2^");
	bl->append_l(bl, fields.exponent);
	bl->append_s(bl, ") * (");
	bl->append_l(bl, fields.significand);
	bl->append_s(bl, " / (2^");
	bl->append_ul(bl, efloat_float_exp_shift);
	bl->append_s(bl, ")))");

	if (written) {
		*written = eembed_strnlen(buf, len);
	}
	return (eembed_strstr(buf, ")))")) ? buf : NULL;
}

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
static int64_t efloat64_to_int64_bits_memcpy(efloat64 f)
{
	int64_t i;
	eembed_memcpy(&i, &f, sizeof(int64_t));
	return i;
}

static int64_t efloat64_to_int64_bits_unionp(efloat64 f)
{
	union efloat64_u {
		efloat64 f;
		int64_t i;
	} pun;
	pun.f = f;
	return pun.i;
}

int64_t efloat64_to_int64_bits(efloat64 f)
{
	if (eembed_memcpy) {
		return efloat64_to_int64_bits_memcpy(f);
	} else {
		return efloat64_to_int64_bits_unionp(f);
	}
}

static uint64_t efloat64_to_uint64_bits_memcpy(efloat64 f)
{
	uint64_t u;
	eembed_memcpy(&u, &f, sizeof(uint64_t));
	return u;
}

static uint64_t efloat64_to_uint64_bits_unionp(efloat64 f)
{
	union efloat64_u {
		efloat64 f;
		uint64_t u;
	} pun;
	pun.f = f;
	return pun.u;
}

uint64_t efloat64_to_uint64_bits(efloat64 f)
{
	if (eembed_memcpy) {
		return efloat64_to_uint64_bits_memcpy(f);
	} else {
		return efloat64_to_uint64_bits_unionp(f);
	}
}

static efloat64 int64_bits_to_efloat64_memcpy(int64_t i)
{
	efloat64 f;
	eembed_memcpy(&f, &i, sizeof(efloat64));
	return f;
}

static efloat64 int64_bits_to_efloat64_unionp(int64_t i)
{
	union efloat64_u {
		efloat64 f;
		int64_t i;
	} pun;
	pun.i = i;
	return pun.f;
}

efloat64 int64_bits_to_efloat64(int64_t i)
{
	if (eembed_memcpy) {
		return int64_bits_to_efloat64_memcpy(i);
	} else {
		return int64_bits_to_efloat64_unionp(i);
	}
}

static efloat64 uint64_bits_to_efloat64_memcpy(uint64_t u)
{
	efloat64 f;
	eembed_memcpy(&f, &u, sizeof(efloat64));
	return f;
}

static efloat64 uint64_bits_to_efloat64_unionp(uint64_t u)
{
	union efloat64_u {
		efloat64 f;
		uint64_t u;
	} pun;
	pun.u = u;
	return pun.f;
}

efloat64 uint64_bits_to_efloat64(uint64_t u)
{
	if (eembed_memcpy) {
		return uint64_bits_to_efloat64_memcpy(u);
	} else {
		return uint64_bits_to_efloat64_unionp(u);
	}
}

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
		Efloat_set_err_inval();
		err = 1;
		raw_exp = efloat64_r2_exp_inf_nan;
	}
	raw_exp = (raw_exp + efloat64_r2_exp_max);

	raw_significand = (fields.exponent == 0)
	    ? (fields.significand >> 1)
	    : (fields.significand);

	if (raw_significand != (raw_significand & efloat64_r2_signif_mask)) {
		Efloat_set_err_inval();
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
			Efloat_set_err_inval();
			Efloat_debug_print_str("sign ");
			Efloat_debug_print_i32(fields.sign);
			Efloat_debug_print_str(" != ");
			Efloat_debug_print_i32(f2.sign);
			Efloat_debug_print_eol();
		}
		if (fields.exponent != f2.exponent) {
			Efloat_set_err_inval();
			Efloat_debug_print_str("exponent ");
			Efloat_debug_print_i32(fields.exponent);
			Efloat_debug_print_str(" != ");
			Efloat_debug_print_i32(f2.exponent);
			Efloat_debug_print_eol();
		}
		if (fields.significand != f2.significand) {
			Efloat_set_err_inval();
			Efloat_set_err_inval();
			Efloat_debug_print_str("exponent ");
			Efloat_debug_print_u64(fields.significand);
			Efloat_debug_print_str(" != ");
			Efloat_debug_print_u64(f2.significand);
			Efloat_debug_print_eol();
		}
	}
	return f;
}

char *efloat64_fields_to_expression(const struct efloat64_fields fields,
				    char *buf, size_t len, int *written)
{
	struct eembed_log log;
	struct eembed_str_buf sbuf;
	struct eembed_log *bl;

	if (buf && len) {
		buf[0] = '\0';
	}
	bl = eembed_char_buf_log_init(&log, &sbuf, buf, len);
	if (!bl) {
		return NULL;
	}

	bl->append_s(bl, "(");
	bl->append_l(bl, fields.sign);
	bl->append_s(bl, " * (2^");
	bl->append_l(bl, fields.exponent);
	bl->append_s(bl, ") * (");
	bl->append_l(bl, fields.significand);
	bl->append_s(bl, " / (2^");
	bl->append_ul(bl, efloat_float_exp_shift);
	bl->append_s(bl, ")))");

	if (written) {
		*written = eembed_strnlen(buf, len);
	}
	return (eembed_strstr(buf, ")))")) ? buf : NULL;
}

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
