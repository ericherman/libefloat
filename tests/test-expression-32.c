/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* test-expression-32.c: test for the Embedable Float manipulation library */
/* Copyright (C) 2018, 2019 Eric Herman */
/* https://github.com/ericherman/libefloat */

#include <errno.h>
#include <float.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "echeck.h"
#include "efloat.h"

ssize_t eval_expression(const char *expression, char *buffer, size_t len)
{
	pid_t pid;
	int filedes[2] = { 0, 0 };
	ssize_t total_cnt = 0;
	ssize_t count = 0;

	if (!buffer) {
		exit(1);
	}
	if (!len) {
		exit(1);
	}

	buffer[0] = '\0';

	if (pipe(filedes) == -1) {
		perror("pipe");
		exit(1);
	}

	pid = fork();
	if (pid == -1) {
		perror("fork");
		exit(1);
	} else if (pid == 0) {
		while ((dup2(filedes[1], STDOUT_FILENO) == -1)
		       && (errno == EINTR)) {
		}
		close(filedes[1]);
		close(filedes[0]);
		execl("./tests/ep", "./tests/ep", expression, (char *)0);
		perror("execl");
		_exit(1);
	}
	close(filedes[1]);

	while (1) {
		count = read(filedes[0], buffer, len);
		if (count < 0) {
			if (errno == EINTR) {
				continue;
			} else {
				perror("read");
				total_cnt = count;
				break;
			}
		} else if (count == 0) {
			break;
		} else {
			total_cnt += count;
			buffer += count;
			if (((signed)len) >= count) {
				len -= count;
			} else {
				len = 0;
				total_cnt = -2;
				break;
			}
		}
	}
	close(filedes[0]);
	wait(0);

	if (len) {
		buffer[len - 1] = '\0';
	}
	if ((total_cnt > 0) && (((uint64_t)total_cnt) < len)) {
		buffer[total_cnt] = '\0';
	} else {
		buffer[0] = '\0';
	}

	return total_cnt;
}

int print_eval_expression(FILE *out, char *expression)
{
	ssize_t written = 0;
	char buf[80];
	size_t len = 80;
	buf[0] = '\0';

	fprintf(out, "%s = ", expression);
	fflush(out);
	written = eval_expression(expression, buf, len);
	fprintf(out, "%s\n", buf);
	fflush(out);
	return written >= 0 ? 0 : 1;
}

void _bogus_chop_trailing_non_num(char *buf, size_t len)
{
	size_t i;
	for (i = 0; i < len && buf[i]; ++i) {
		switch (buf[i]) {
		case '-':
		case '.':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			break;
		default:
			buf[i] = '\0';
		}
	}
	buf[i] = '\0';
}

int efloat32_expression_round_trip(efloat32 f)
{
	double d;
	efloat32 f2 = 0.0;
	efloat32 ep1 = 0.0001;
	efloat32 ep2 = 2 * FLT_EPSILON;
	efloat32 epsilon = FLT_EPSILON;
	struct efloat32_fields fields = { 0, 0, 0 };
	struct efloat32_fields fields2 = { 0, 0, 0 };
	char expression[80];
	char expression2[80];
	char result[80];
	char msg[500];
	int written = 0;
	expression[0] = '\0';
	result[0] = '\0';
	msg[0] = '\0';

	efloat32_radix_2_to_fields(f, &fields);
	efloat32_fields_to_expression(fields, expression, 80, &written);
	if (written <= 0) {
		exit(1);
	}

	written = eval_expression(expression, result, 80);
	if (written <= 0) {
		snprintf(msg, 500, "%f, %s = %s", f, expression, result);
		fprintf(stderr, "%s\n", msg);
		exit(1);
	}
	_bogus_chop_trailing_non_num(result, 80);
	sscanf(result, "%lf", &d);
	f2 = d;

	efloat32_radix_2_to_fields(f2, &fields2);
	efloat32_fields_to_expression(fields2, expression2, 80, &written);
	if (written <= 0) {
		exit(1);
	}

	snprintf(msg, 500, "f: %g, %s = %s (%g, %s)\n", f, expression, result,
		 f2, expression2);
	ep2 = (fabs(f) * FLT_EPSILON * 2);
	epsilon = (ep1 > ep2) ? ep1 : ep2;
	return check_double_m(f, f2, epsilon, msg);
}

int uint32_efloat32_expression_round_trip(uint32_t u, uint64_t *cnt)
{
	efloat32 f;

	f = uint32_bits_to_efloat32(u);
	if ((FP_NORMAL == fpclassify(f) || (FP_ZERO == fpclassify(f)))) {
		++(*cnt);
		return efloat32_expression_round_trip(f);
	}
	return 0;
}

int main(int argc, char **argv)
{
	int stepi;
	uint32_t i, step, limit, val;
	uint64_t err, cnt;

	if (sizeof(efloat32) != sizeof(uint32_t)) {
		fprintf(stderr,
			"sizeof(efloat32) %lu != sizeof(uint32_t) %lu!\n",
			(unsigned long)sizeof(efloat32),
			(unsigned long)sizeof(uint32_t));
		return EXIT_FAILURE;
	}

	stepi = argc > 1 ? atoi(argv[1]) : 0;

	if (stepi <= 0) {
		step = (UINT32_MAX / 500);
	} else {
		step = stepi;
	}
	limit = (UINT32_MAX / step);

	cnt = 0;
	err = 0;

	err += uint32_efloat32_expression_round_trip(UINT32_MAX, &cnt);
	err += uint32_efloat32_expression_round_trip(limit, &cnt);

	for (i = 0; i < limit; ++i) {
		val = (i * step);
		err += uint32_efloat32_expression_round_trip(val, &cnt);
	}
	return (err == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
