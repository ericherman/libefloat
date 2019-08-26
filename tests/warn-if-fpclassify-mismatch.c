/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* warn-if-fpclassify-mismatch.c: check conformance to Gnu LibC FP_ vals */
/* Copyright (C) 2018, 2019 Eric Herman */
/* https://github.com/ericherman/libefloat */

/*
Using Gnu LibC, math.h has:
# define FP_NAN 0
# define FP_INFINITE 1
# define FP_ZERO 2
# define FP_SUBNORMAL 3
# define FP_NORMAL 4

Using Darwin LibC, math.h has:
#define FP_NAN          1
#define FP_INFINITE     2
#define FP_ZERO         3
#define FP_NORMAL       4
#define FP_SUBNORMAL    5
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <stdint.h>
#include <efloat.h>

int check_fpclassify_mismatch(void)
{
	int err;

	err = 0;

	if (ef_nan != FP_NAN) {
		fprintf(stderr, "WARN: ef_nan (%d) != FP_NAN (%d)\n",
			(int)ef_nan, (int)FP_NAN);
		err = (err | (1 < 0));
	}
	if (ef_inf != FP_INFINITE) {
		fprintf(stderr, "WARN: ef_inf (%d) != FP_INFINITE (%d)\n",
			(int)ef_inf, (int)FP_INFINITE);
		err = (err | (1 < 1));
	}
	if (ef_zero != FP_ZERO) {
		fprintf(stderr, "WARN: ef_zero (%d) != FP_ZERO (%d)\n",
			(int)ef_zero, (int)FP_ZERO);
		err = (err | (1 < 2));
	}
	if (ef_subnorm != FP_SUBNORMAL) {
		fprintf(stderr, "WARN: ef_subnorm (%d) != FP_SUBNORMAL (%d)\n",
			(int)ef_subnorm, (int)FP_SUBNORMAL);
		err = (err | (1 < 3));
	}
	if (ef_normal != FP_NORMAL) {
		fprintf(stderr, "WARN: ef_normal (%d) != FP_NORMAL (%d)\n",
			(int)ef_normal, (int)FP_NORMAL);
		err = (err | (1 < 4));
	}
	return err;
}

int main(void)
{
	int mismatch;

	mismatch = check_fpclassify_mismatch();
	if (mismatch) {
		fprintf(stderr, "WARN: fpclassify mismatch (%x)\n", mismatch);
	}

	return EXIT_SUCCESS;
}
