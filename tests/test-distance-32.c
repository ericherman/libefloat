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

int check_efloat32_distance(efloat32 x, efloat32 y, uint32_t expect,
			    int verbose)
{
	uint32_t xi, yi, distance;
	int err;

	distance = efloat32_distance(x, y);
	err = (distance == expect) ? 0 : 1;
	if (err || verbose > 1) {
		xi = efloat32_to_int32(x);
		yi = efloat32_to_int32(y);
		fprintf(stderr,
			"%g (%ld), %g (%ld), reports distance of %lu,"
			" expected %lu\n", (double)x, (long)xi, (double)y,
			(long)yi, (unsigned long)distance,
			(unsigned long)expect);
	}
	return err;
}

uint64_t check_efloat32_between(efloat32 from, efloat32 until, size_t step,
				int verbose, uint64_t *cnt)
{
	size_t i;
	efloat32 f, n, na;
	int out_of_bounds;
	uint64_t err;

	*cnt = 0;
	err = 0;
	out_of_bounds = 0;
	for (f = from;
	     (from < until ? (f < until) : (f > until)) && !out_of_bounds;
	     f = n) {
		++(*cnt);
		n = f;
		for (i = 0; i < step; ++i) {
			na = nextafter32(n, until);
			if (na == n) {
				out_of_bounds = 1;
			} else {
				n = na;
			}
		}
		if (!isfinite(n)) {
			out_of_bounds = 1;
		}
		if (!out_of_bounds) {
			err += check_efloat32_distance(f, n, step, verbose);
		}
	}
	return err;
}

int main(int argc, char **argv)
{
	size_t step;
	uint64_t err, cnt_0_to_1, cnt;
	int verbose, argstep;

	verbose = argc > 1 ? atoi(argv[1]) : 0;
	argstep = argc > 2 ? atoi(argv[2]) : 0;

	err = 0;
	cnt = 0;

	step = 1;
	err += check_efloat32_distance(0.0, sqrt(-1), UINT32_MAX, verbose);
	err += check_efloat32_distance(sqrt(-1), sqrt(-1), UINT32_MAX, verbose);
	err +=
	    check_efloat32_distance(sqrt(-1), -(sqrt(-1)), UINT32_MAX, verbose);
	err += check_efloat32_distance(INFINITY, INFINITY, 0, verbose);
	err += check_efloat32_distance(-INFINITY, -INFINITY, 0, verbose);
	err +=
	    check_efloat32_distance(INFINITY, -INFINITY, UINT32_MAX, verbose);
	if (err) {
		exit(EXIT_FAILURE);
	}
	err += check_efloat32_between(0.0, 1.0, step, verbose, &cnt_0_to_1);
	err += check_efloat32_between(0.0, -1.0, step, verbose, &cnt);
	if (cnt_0_to_1 != cnt) {
		++err;
		fprintf(stderr, "iterations between 0.0 and +1.0 = %llu\n",
			(unsigned long long)cnt_0_to_1);
		fprintf(stderr, "iterations between 0.0 and -1.0 = %llu\n",
			(unsigned long long)cnt);
	}
	err += check_efloat32_distance(0.0, 1.0, cnt_0_to_1, verbose);
	err += check_efloat32_distance(0.0, -1.0, cnt_0_to_1, verbose);
	err += check_efloat32_distance(-1.0, 1.0, (2 * cnt_0_to_1), verbose);

	step = argstep > 0 ? (size_t)argstep : 0;
	if (step) {
		err +=
		    check_efloat32_between(-efloat32_max, efloat32_max, step,
					   verbose, &cnt);
	}

	if (verbose || err) {
		fprintf(stderr, "attempted %lu values, %lu errors\n",
			(unsigned long)cnt, (unsigned long)err);
	}
	return (err == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
