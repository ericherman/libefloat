# Makefile: Embedable Float manipulation library
# Copyright (C) 2017 Eric Herman
#
# https://github.com/ericherman/libefloat
#
# This work is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later
# version.

SHELL=/bin/bash
UNAME := $(shell uname)

EXPRESSION_PARSER=bc -l <<<

ifeq ($(UNAME), Darwin)
SHAREDFLAGS = -dynamiclib
SHAREDEXT = dylib
else
SHAREDFLAGS = -shared
SHAREDEXT = so
endif

ifeq ("$(PREFIX)", "")
PREFIX=/usr/local
endif

ifeq ("$(LIBDIR)", "")
LIBDIR=$(PREFIX)/lib
endif

ifeq ("$(INCDIR)", "")
INCDIR=$(PREFIX)/include
endif

ifneq ($(strip $(srcdir)),)
   VPATH::=$(srcdir)
endif

CC=gcc

SYSTEM_CONFIG_CFLAGS= \
 -DHAVE_STDINT_H=1 \
 -DHAVE_LIMITS_H=1 \
 -DHAVE_FLOAT_H=1 \
 -DHAVE_MEMCPY=1 \
 -DHAVE_STRING_H=1 \
 -DHAVE_STDIO_H=1 \
 -DHAVE_ERRNO=1 \
 -DHAVE_ERRNO_H=1

# typical
BUILD_TYPE_CFLAGS=-g -O2 -fomit-frame-pointer -DNDEBUG
# profiling
#BUILD_TYPE_CFLAGS=-g -O2 -pg -DNDEBUG
# debug
#BUILD_TYPE_CFLAGS=-g -O0 -DDEBUG=1

# we may use "long long" for uint64_t (x86 32bit)
CSTD_CFLAGS=-std=c89 -pedantic -Wno-long-long

NOISY_CFLAGS=-Werror -Wall -Wextra -Werror=cast-qual

INCLUDES_CFLAGS=-I./src

BASE_CFLAGS=$(CFLAGS) \
 $(NOISY_CFLAGS) \
 $(BUILD_TYPE_CFLAGS) \
 $(INCLUDES_CFLAGS)

LIB_CFLAGS=$(CSTD_CFLAGS) $(BASE_CFLAGS) $(SYSTEM_CONFIG_CFLAGS)

# tests need "gnu89" for fpclassify
TEST_CSTD_CFLAGS=-std=gnu89 -pedantic -Wno-long-long

TEST_CFLAGS=$(TEST_CSTD_CFLAGS) $(BASE_CFLAGS)
TEST_LDFLAGS=$(LDFLAGS) -L.
TEST_LDADD=$(LDADD) -lm -lefloat

EFLT_LIB_SRC=src/efloat.c
EFLT_LIB_HDR=src/efloat.h
EFLT_LIB_OBJ=efloat.o

LIB_NAME=libefloat

SO_OBJS=$(EFLT_LIB_OBJ)
SO_NAME=$(LIB_NAME).$(SHAREDEXT)
ifneq ($(UNAME), Darwin)
    SHAREDFLAGS += -Wl,-soname,$(SO_NAME)
endif

A_NAME=libefloat.a

TEST_RT_32_SRC=tests/test-round-trip-32.c
TEST_RT_32_OBJ=test-round-trip-32.o
TEST_RT_32_EXE=test-round-trip-32

TEST_RT_64_SRC=tests/test-round-trip-64.c
TEST_RT_64_OBJ=test-round-trip-64.o
TEST_RT_64_EXE=test-round-trip-64

TEST_DIST_32_SRC=tests/test-distance-32.c
TEST_DIST_32_OBJ=test-distance-32.o
TEST_DIST_32_EXE=test-distance-32

TEST_DIST_64_SRC=tests/test-distance-64.c
TEST_DIST_64_OBJ=test-distance-64.o
TEST_DIST_64_EXE=test-distance-64

TEST_DEMO_SRC=demo/libefloat-demo.c
TEST_DEMO_EXE=libefloat-demo

default: library

$(EFLT_LIB_OBJ): $(EFLT_LIB_HDR) $(EFLT_LIB_SRC)
	$(CC) -c -fPIC $(LIB_CFLAGS) $(EFLT_LIB_SRC) -o $(EFLT_LIB_OBJ)

$(SO_NAME): $(SO_OBJS)
	$(CC) $(SHAREDFLAGS) -o $(SO_NAME).1.0 $(SO_OBJS)
	ln -sf ./$(SO_NAME).1.0 ./$(SO_NAME).1
	ln -sf ./$(SO_NAME).1.0 ./$(SO_NAME)

$(A_NAME): $(SO_OBJS)
	ar -r $(A_NAME) $(SO_OBJS)

$(LIB_NAME): $(SO_NAME) $(A_NAME)

warn-if-fpclassify-mismatch: $(A_NAME) tests/warn-if-fpclassify-mismatch.c
	$(CC) $(TEST_CFLAGS) $(A_NAME) tests/warn-if-fpclassify-mismatch.c \
		-o warn-if-fpclassify-mismatch
	./warn-if-fpclassify-mismatch

$(TEST_RT_32_OBJ): $(EFLT_LIB_HDR) $(TEST_RT_32_SRC)
	$(CC) -c $(TEST_CFLAGS) $(TEST_RT_32_SRC) -o $(TEST_RT_32_OBJ)

$(TEST_RT_32_EXE)-static: $(TEST_RT_32_OBJ) $(A_NAME)
	$(CC) $(TEST_RT_32_OBJ) $(A_NAME) -o $(TEST_RT_32_EXE)-static

$(TEST_RT_32_EXE)-dynamic: $(TEST_RT_32_OBJ) $(SO_NAME)
	$(CC) $(TEST_RT_32_OBJ) $(TEST_LDFLAGS) \
		-o $(TEST_RT_32_EXE)-dynamic $(TEST_LDADD)

$(TEST_DIST_32_OBJ): $(EFLT_LIB_HDR) $(TEST_DIST_32_SRC)
	$(CC) -c $(TEST_CFLAGS) $(TEST_DIST_32_SRC) -o $(TEST_DIST_32_OBJ)

$(TEST_DIST_32_EXE)-dynamic: $(TEST_DIST_32_OBJ) $(SO_NAME)
	$(CC) $(TEST_DIST_32_OBJ) $(TEST_LDFLAGS) \
		-o $(TEST_DIST_32_EXE)-dynamic $(TEST_LDADD)

check-32-static: $(TEST_RT_32_EXE)-static warn-if-fpclassify-mismatch
	./$(TEST_RT_32_EXE)-static

check-32-dynamic: $(TEST_RT_32_EXE)-dynamic $(TEST_DIST_32_EXE)-dynamic \
	warn-if-fpclassify-mismatch
	LD_LIBRARY_PATH=. ./$(TEST_RT_32_EXE)-dynamic
	LD_LIBRARY_PATH=. ./$(TEST_DIST_32_EXE)-dynamic

check-32: check-32-static check-32-dynamic

# this will check all 32bit values for round-trip success
check-32-exhaustive: $(TEST_RT_32_EXE)-static
	time ./$(TEST_RT_32_EXE)-static 1 1
	time LD_LIBRARY_PATH=. ./$(TEST_DIST_32_EXE)-dynamic 0 1

$(TEST_RT_64_OBJ): $(EFLT_LIB_HDR) $(TEST_RT_64_SRC)
	$(CC) -c $(TEST_CFLAGS) $(TEST_RT_64_SRC) -o $(TEST_RT_64_OBJ)

$(TEST_RT_64_EXE)-static: $(TEST_RT_64_OBJ) $(A_NAME)
	$(CC) $(TEST_RT_64_OBJ) $(A_NAME) -o $(TEST_RT_64_EXE)-static

$(TEST_RT_64_EXE)-dynamic: $(TEST_RT_64_OBJ) $(SO_NAME)
	$(CC) $(TEST_RT_64_OBJ) $(TEST_LDFLAGS) \
		-o $(TEST_RT_64_EXE)-dynamic $(TEST_LDADD)

$(TEST_DIST_64_OBJ): $(EFLT_LIB_HDR) $(TEST_DIST_64_SRC)
	$(CC) -c $(TEST_CFLAGS) $(TEST_DIST_64_SRC) -o $(TEST_DIST_64_OBJ)

$(TEST_DIST_64_EXE)-dynamic: $(TEST_DIST_64_OBJ) $(SO_NAME)
	$(CC) $(TEST_DIST_64_OBJ) $(TEST_LDFLAGS) \
		-o $(TEST_DIST_64_EXE)-dynamic $(TEST_LDADD)

check-64-static: $(TEST_RT_64_EXE)-static warn-if-fpclassify-mismatch
	./$(TEST_RT_64_EXE)-static

check-64-dynamic: $(TEST_RT_64_EXE)-dynamic $(TEST_DIST_64_EXE)-dynamic \
	warn-if-fpclassify-mismatch
	LD_LIBRARY_PATH=. ./$(TEST_RT_64_EXE)-dynamic
	LD_LIBRARY_PATH=. ./$(TEST_DIST_64_EXE)-dynamic

check-64: check-64-static check-64-dynamic

check-static: check-32-static check-64-static

check-dynamic: check-32-dynamic check-64-dynamic

echo_makeflags:
	echo "MAKEFLAGS='$(MAKEFLAGS)'"

check: echo_makeflags check-32 check-64
	@echo "success"

valgrind-32: ./$(TEST_RT_32_EXE)-static
	valgrind ./$(TEST_RT_32_EXE)-static

valgrind-64: ./$(TEST_RT_64_EXE)-static
	valgrind ./$(TEST_RT_64_EXE)-static

valgrind: valgrind-32 valgrind-64

float-to-fields: $(A_NAME) demo/ehstr.h demo/ehstr.c
	$(CC) $(SYSTEM_CONFIG_CFLAGS) \
		$(TEST_CFLAGS) \
		-I./demo \
		src/efloat.c \
		demo/ehstr.c \
		demo/float-to-fields.c -o float-to-fields

double-to-fields: $(A_NAME) demo/ehstr.h demo/ehstr.c
	$(CC) $(SYSTEM_CONFIG_CFLAGS) \
		$(TEST_CFLAGS) \
		-I./demo \
		src/efloat.c \
		demo/ehstr.c \
		demo/double-to-fields.c -o double-to-fields

fields-to-float: $(A_NAME) demo/ehstr.h demo/ehstr.c
	$(CC) $(SYSTEM_CONFIG_CFLAGS) \
		$(TEST_CFLAGS) \
		-I./demo \
		src/efloat.c \
		demo/ehstr.c \
		demo/fields-to-float.c -o fields-to-float

fields-to-double: $(A_NAME) demo/ehstr.h demo/ehstr.c
	$(CC) $(SYSTEM_CONFIG_CFLAGS) \
		$(TEST_CFLAGS) \
		-I./demo \
		src/efloat.c \
		demo/ehstr.c \
		demo/fields-to-double.c -o fields-to-double

$(TEST_DEMO_EXE): $(A_NAME) $(EFLT_LIB_HDR) $(TEST_DEMO_SRC)
	$(CC) $(TEST_CFLAGS) $(TEST_DEMO_SRC) $(A_NAME) -o $(TEST_DEMO_EXE)

demo: $(TEST_DEMO_EXE) \
		float-to-fields double-to-fields \
		fields-to-float fields-to-double
	./$(TEST_DEMO_EXE) 1
	@echo
	./$(TEST_DEMO_EXE) 0.5
	@echo
	./$(TEST_DEMO_EXE) 0.0
	@echo
	./$(TEST_DEMO_EXE) -0.0
	@echo
	./$(TEST_DEMO_EXE) 0.1
	@echo
	./$(TEST_DEMO_EXE) -0.00004211
	@echo
	./float-to-fields -1234.567
	@echo
	./fields-to-float -1 10 10113573
	($(EXPRESSION_PARSER) '-1 * (2^10) * (10113573 / (2^23))')
	@echo
	./double-to-fields -1234.567
	@echo
	./fields-to-double -1 10 5429683087074132
	($(EXPRESSION_PARSER) '-1 * (2^10) * (5429683087074132 / (2^52))')

# extracted from https://github.com/torvalds/linux/blob/master/scripts/Lindent
LINDENT=indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 -il0

tidy:
	patch -Np1 -i misc/pre-tidy.patch
	$(LINDENT) \
		-T efloat32 -T efloat64 -T efloat_class \
		-T FILE -T size_t -T ssize_t \
		-T uint8_t -T uint16_t -T uint32_t -T uint64_t \
		-T int8_t -T int16_t -T int32_t -T int64_t \
		`find src tests demo -name '*.h' -o -name '*.c'`
	patch -Rp1 -i misc/pre-tidy.patch

spotless:
	git clean -dxf

clean:
	rm -rvf `cat .gitignore | sed -e 's/#.*//'`
	pushd src && rm -rvf `cat ../.gitignore | sed -e 's/#.*//'`; popd
	pushd tests && rm -rvf `cat ../.gitignore | sed -e 's/#.*//'`; popd

library: $(LIB_NAME)

install: library
	 @echo "Installing libraries in $(LIBDIR)"
	 mkdir -pv $(LIBDIR)/
	 cp -pv $(A_NAME) $(LIBDIR)/
	 cp -Rv $(SO_NAME)* $(LIBDIR)/
	 @echo "Installing headers in $(INCDIR)"
	 mkdir -pv $(INCDIR)/
	 cp -pv $(EFLT_LIB_HDR) $(INCDIR)/
