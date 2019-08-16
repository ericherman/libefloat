/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* libecheck: a few handy stirng functions */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#ifndef EHSTR_H
#define EHSTR_H

#ifdef __cplusplus
#define Ehstr_begin_C_functions extern "C" {
#define Ehstr_end_C_functions }
#else
#define Ehstr_begin_C_functions
#define Ehstr_end_C_functions
#endif

#include <stddef.h>		/* size_t */

Ehstr_begin_C_functions
#undef Ehstr_begin_C_functions
/*
  unsigned to big-endian binary
  represents a binary value as a string of zeros and ones
  see also: strtoul(buf, NULL, 2);
*/
char *utob(char *buf, size_t buf_szie, unsigned long val, size_t bits);

/*
  reverse a string in place
  e.g. to convert "0111" from big-endian to little-endian "1110"
*/
void revstr(char *str, size_t buf_size);

/* trim leading and trailing whitespace from a string in place */
void trimstr(char *str, size_t len);

/* converts an arbitrarily long string of unsigned decimal digits into hex */
char *decimal_to_hex(const char *dec_str, size_t dec_len, char *buf,
		     size_t buf_len);

/* converts an arbitrarily long string of hex digits into unsigned decimal */
char *hex_to_decimal(const char *hex_str, size_t hex_len, char *buf,
		     size_t buf_len);

void byte_to_hex_chars(unsigned char byte, char *high, char *low);

unsigned char hex_chars_to_byte(char high, char low);

/*
  mostly for c89 compat

  return the length of str, but check only as many as buf_size;
  if a '\0' is not found in the first buf_size, return buf_size
*/
size_t ehstrnlen(const char *str, size_t buf_size);
#if _XOPEN_SOURCE < 700 && _POSIX_C_SOURCE < 200809L
#define strnlen ehstrnlen
#else
#include <string.h>
#endif

Ehstr_end_C_functions
#undef Ehstr_end_C_functions
#endif /* EHSTR_H */
