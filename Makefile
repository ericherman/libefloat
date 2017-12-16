check:
	gcc -std=gnu89 -pedantic \
		-g -O2 -fomit-frame-pointer \
		-Wall -Wextra -Werror -Wno-long-long \
		-DHAVE_STDINT_H=1 \
		-DHAVE_FLOAT_H=1 \
		-DHAVE_LIMITS_H=1 \
		-DHAVE_STDIO_H=1 \
		-I./src \
		-o ./test-round-trip \
		./src/efloat.c ./tests/test-round-trip.c \
		-lm
	./test-round-trip

clean:
	git clean -dxf
