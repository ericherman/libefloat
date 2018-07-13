/*
test-distance-64.c: quick test for the Embedable Float manipulation library
Copyright (C) 2017, 2018 Eric Herman

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

int check_efloat64_distance(efloat64 x, efloat64 y, uint64_t expect,
			    int verbose)
{
	uint64_t xi, yi, distance;
	int err;

	distance = efloat64_distance(x, y);
	err = (distance == expect) ? 0 : 1;
	if (err || verbose > 1) {
		xi = efloat64_to_int64_bits(x);
		yi = efloat64_to_int64_bits(y);
		fprintf(stderr,
			"%g (%ld), %g (%ld), reports distance of %lu,"
			" expected %lu\n", (double)x, (long)xi, (double)y,
			(long)yi, (unsigned long)distance,
			(unsigned long)expect);
	}
	return err;
}

uint64_t check_efloat64_between(efloat64 from, efloat64 until, size_t step,
				int verbose, uint64_t *cnt)
{
	size_t i;
	efloat64 f, n, na;
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
			na = nextafter64(n, until);
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
			err += check_efloat64_distance(f, n, step, verbose);
		}
	}
	return err;
}

int main(int argc, char **argv)
{
	size_t i, step;
	efloat64 mark;
	uint64_t err, cnt_0_to_mark, cnt;
	int verbose, argstep;

	verbose = argc > 1 ? atoi(argv[1]) : 0;
	argstep = argc > 2 ? atoi(argv[2]) : 0;

	err = 0;
	cnt = 0;

	step = 1;
	err += check_efloat64_distance(0.0, sqrt(-1), UINT64_MAX, verbose);
	err += check_efloat64_distance(sqrt(-1), sqrt(-1), UINT64_MAX, verbose);
	err +=
	    check_efloat64_distance(sqrt(-1), -(sqrt(-1)), UINT64_MAX, verbose);
	err += check_efloat64_distance(INFINITY, INFINITY, 0, verbose);
	err += check_efloat64_distance(-INFINITY, -INFINITY, 0, verbose);
	err +=
	    check_efloat64_distance(INFINITY, -INFINITY, UINT64_MAX, verbose);
	if (err) {
		exit(EXIT_FAILURE);
	}
	mark = efloat64_min;
	for (i = 0; i < 10; ++i) {
		mark = nextafter(mark, efloat64_max);
	}
	if (verbose
	    || labs(efloat64_to_int64_bits(mark) -
		    efloat64_to_int64_bits(0.0)) > 10 * 1000 * 1000) {
		fprintf(stderr, "%g (%ld), %g (%ld), difference = %ld?\n",
			(double)mark, (long)efloat64_to_int64_bits(mark),
			(double)0.0, (long)efloat64_to_int64_bits(0.0),
			labs(efloat64_to_int64_bits(mark) -
			     efloat64_to_int64_bits(0.0)));
		mark = nextafter(0.0, efloat64_max);
		for (i = 0; i < 10; ++i) {
			mark = nextafter(mark, efloat64_max);
		}
		fprintf(stderr, "using %g (%ld) instead\n", mark,
			(long)efloat64_to_int64_bits(mark));
		fprintf(stderr, "%g (%ld), %g (%ld), difference = %ld\n",
			(double)mark, (long)efloat64_to_int64_bits(mark),
			(double)0.0, (long)efloat64_to_int64_bits(0.0),
			labs(efloat64_to_int64_bits(mark) -
			     efloat64_to_int64_bits(0.0)));
	}

	err += check_efloat64_between(0.0, mark, step, verbose, &cnt_0_to_mark);
	err += check_efloat64_between(0.0, -mark, step, verbose, &cnt);
	if (cnt_0_to_mark != cnt) {
		++err;
		fprintf(stderr, "iterations between 0.0 and +mark = %llu\n",
			(unsigned long long)cnt_0_to_mark);
		fprintf(stderr, "iterations between 0.0 and -mark = %llu\n",
			(unsigned long long)cnt);
	}
	err += check_efloat64_distance(0.0, mark, cnt_0_to_mark, verbose);
	err += check_efloat64_distance(0.0, -mark, cnt_0_to_mark, verbose);
	err +=
	    check_efloat64_distance(-mark, mark, (2 * cnt_0_to_mark), verbose);

	step = argstep > 0 ? (size_t)argstep : 0;
	if (step) {
		err +=
		    check_efloat64_between(-efloat64_max, efloat64_max, step,
					   verbose, &cnt);
	}

	if (verbose || err) {
		fprintf(stderr, "attempted %lu values, %lu errors\n",
			(unsigned long)cnt, (unsigned long)err);
	}
	return (err == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
