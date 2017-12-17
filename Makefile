check: check-test-round-trip-64

test-round-trip-64:
	gcc -std=gnu89 -pedantic \
		-g -O2 -fomit-frame-pointer \
		-Wall -Wextra -Werror -Wno-long-long \
		-DHAVE_STDINT_H=1 \
		-DHAVE_FLOAT_H=1 \
		-DHAVE_LIMITS_H=1 \
		-DHAVE_STDIO_H=1 \
		-I./src \
		-o ./test-round-trip-64 \
		./src/efloat.c ./tests/test-round-trip-64.c \
		-lm

check-test-round-trip-64: test-round-trip-64
	./test-round-trip-64

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


clean:
	git clean -dxf
