/*
test-distance-32.c: quick test for the Embedable Float manipulation library
Copyright (C) 2017 Eric Herman

https://github.com/ericherman/libefloat

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later
version.
*/

#include <stdint.h>
#include <float.h>
#include <efloat.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char **argv)
{
	size_t i, step;
	int32_t fi, ni;
	uint32_t distance;
	efloat32 f, n, na;
	uint64_t err, cnt;
	int verbose, argstep, print;

	verbose = argc > 1 ? atoi(argv[1]) : 0;
	argstep = argc > 2 ? atoi(argv[2]) : 0;

	err = 0;
	cnt = 0;
	step = argstep > 0 ? (size_t)argstep : 1U;

	for (f = -efloat32_max; f < efloat32_max; f = n) {
		++cnt;
		n = f;
		for (i = 0; i < step; ++i) {
			na = nextafter32(n, efloat32_max);
			if (na == n) {
				n = sqrt(-1);
			} else {
				n = na;
			}
		}
		if (!isfinite(n)) {
			break;
		}
		fi = efloat32_to_int32(f);
		ni = efloat32_to_int32(n);
		distance = efloat32_distance(f, n);
		if (distance != step) {
			print = 1;
			++err;
		} else {
			print = 0;
		}
		if (verbose > 1 || print) {
			printf
			    ("%lu: %g (%ld), %g (%ld), reports distance of %lu, expected %lu\n",
			     cnt, f, (long)fi, n, (long)ni,
			     (unsigned long)distance, (unsigned long)step);
		}
	}
	if (verbose || err) {
		printf("attempted %lu values, %lu errors\n", (unsigned long)cnt,
		       (unsigned long)err);
	}
	return (err == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
