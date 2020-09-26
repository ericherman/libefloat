/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* libefloat-demo.c : a quick demo of libefloat usage */
/* Copyright (C) 2018, 2019 Eric Herman */
/* https://github.com/ericherman/libefloat */
/*
gcc -DHAVE_ERRNO=1 -DHAVE_ERRNO_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_MEMCPY=1 \
 -DHAVE_STRING_H=1 -DHAVE_SATDIO_H=1 -DHAVE_SNPRINTF=1 \
 -I./src src/efloat.h src/efloat.c demo/libefloat-demo.c -o libefloat-demo

$ ./libefloat-demo
double: 0.333333 => uint64_t: 3fd5555555555555
uint64_t: 3fd5555555555555 => double: 0.333333
float: 0.333333 => uint32_t: 3eaaaaab
uint32_t: 3eaaaaab => float: 0.333333
$
$ ./libefloat-demo -0.0
double: -0 => uint64_t: 8000000000000000
uint64_t: 8000000000000000 => double: -0
float: -0 => uint32_t: 80000000
uint32_t: 80000000 => float: -0
$

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <stdint.h>
#include <efloat.h>
#include <assert.h>

int main(int argc, char **argv)
{
	double f64, f64r;
	uint64_t u64;

	float f32, f32r;
	uint32_t u32;

	double val = argc > 1 ? strtod(argv[1], NULL) : (1.0 / 3.0);

	assert(sizeof(double) == sizeof(uint64_t));
	f64 = val;
	u64 = efloat64_to_uint64_bits(f64);
	printf("double: %g => uint64_t: %llx\n", (double)f64,
	       (unsigned long long)u64);
	f64r = uint64_bits_to_efloat64(u64);
	printf("uint64_t: %llx => double: %g\n",
	       (unsigned long long)u64, (double)f64r);

	assert(sizeof(float) == sizeof(uint32_t));
	f32 = (float)val;
	u32 = efloat32_to_uint32_bits(f32);
	printf("float: %g => uint32_t: %lx\n", (double)f32, (unsigned long)u32);
	f32r = uint32_bits_to_efloat32(u32);
	printf("uint32_t: %lx => float: %g\n",
	       (unsigned long)u32, (double)f32r);

	return EXIT_SUCCESS;
}
