CC=gcc

# -g -O2 -pg \

CFLAGS=-std=gnu89 -pedantic \
 -Wall -Wextra -Werror -Wno-long-long \
 -g -O2 -fomit-frame-pointer \
 -DHAVE_MEMCPY=1 \
 -DHAVE_STDINT_H=1 \
 -DHAVE_FLOAT_H=1 \
 -DHAVE_LIMITS_H=1 \
 -DHAVE_STDIO_H=1 \
 -I./src
LD_ADD=-lm

SHELL=/bin/bash

check: check-32 check-64

test-round-trip-64: ./src/efloat.h ./src/efloat.c ./tests/test-round-trip-64.c
	$(CC) $(CFLAGS) \
		-o ./test-round-trip-64 \
		./src/efloat.c \
		./tests/test-round-trip-64.c \
		$(LD_ADD)

check-64: test-round-trip-64
	time ./test-round-trip-64 1

test-round-trip-32: ./src/efloat.h ./src/efloat.c ./tests/test-round-trip-32.c
	$(CC) $(CFLAGS) \
		-o ./test-round-trip-32 \
		./src/efloat.c \
		./tests/test-round-trip-32.c \
		$(LD_ADD)

check-32: test-round-trip-32
	time ./test-round-trip-32 1

# this will check all 32bit values for round-trip success
check-32-exhaustive: test-round-trip-32
	time ./test-round-trip-32 1 1

# extracted from https://github.com/torvalds/linux/blob/master/scripts/Lindent
LINDENT=indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 -il0

tidy:
	patch -Np1 -i misc/pre-tidy.patch
	$(LINDENT) \
		-T efloat32 -T efloat64 -T efloat_class \
		-T FILE -T size_t -T ssize_t \
		-T uint8_t -T uint16_t -T uint32_t -T uint64_t \
		-T int8_t -T int16_t -T int32_t -T int64_t \
		`find src tests -name '*.h' -o -name '*.c'`
	patch -Rp1 -i misc/pre-tidy.patch


spotless:
	git clean -dxf

clean:
	rm -rvf `cat .gitignore | sed -e 's/#.*//'`
	pushd src && rm -rvf `cat ../.gitignore | sed -e 's/#.*//'`; popd
	pushd tests && rm -rvf `cat ../.gitignore | sed -e 's/#.*//'`; popd
