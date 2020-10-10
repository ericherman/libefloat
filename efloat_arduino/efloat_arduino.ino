/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* efloat_arduino.ino : a quick demo of libefloat usage in arduino*/
/* Copyright (C) 2018, 2019, 2020 Eric Herman */
/* https://github.com/ericherman/libefloat */

#include <Arduino.h>
#include <HardwareSerial.h>

#include "efloat.h"

/* globals */
uint32_t loop_count;

void setup(void)
{
	Serial.begin(9600);

	delay(50);

	Serial.println("Begin");

	loop_count = 0;
}

void loop(void)
{
	Serial.println("=================================================");
	++loop_count;

	Serial.print("sizeof(float) == ");
	Serial.println(sizeof(float));

	Serial.print("sizeof(double) == ");
	Serial.println(sizeof(double));

	Serial.print("sizeof(long double) == ");
	Serial.println(sizeof(long double));

	Serial.print("sizeof(efloat32) == ");
	Serial.println(sizeof(efloat32));

	Serial.print("sizeof(uint32_t) == ");
	Serial.println(sizeof(uint32_t));

	Serial.println();
	float denominator = (loop_count * 1.0);
	float f32 = (1.0 / denominator);
	while (f32 < 0.02) {
		f32 *= 1024.0;
	}
	if ((loop_count > 8) && (loop_count % 2)) {
		f32 = 1.0 / f32;
	}
	if ((loop_count % 3) == 0) {
		f32 = -f32;
	}

	Serial.print("f32 == ");
	Serial.println(f32, 5);
	Serial.println();
	Serial.print("uint32_t u32 = efloat32_to_uint32_bits(");
	Serial.print(f32, 5);
	Serial.println(")");

	uint32_t u32 = efloat32_to_uint32_bits(f32);
	Serial.print("u32 == ");
	Serial.println(u32);

	Serial.println();
	Serial.print("float f32r = uint32_bits_to_efloat32(");
	Serial.print(u32);
	Serial.println(")");
	float f32r = uint32_bits_to_efloat32(u32);
	Serial.print("f32r == ");
	Serial.println(f32r, 5);

	Serial.println();
	Serial.print("Roundtrip ");
	const char *result;
	const char *symbol;
	if (f32 == f32r) {
		symbol = " == ";
		result = "(f32 == f32r) SUCCESS";
	} else {
		result = "(f32 != f32r) FAIL!";
		symbol = " != ";
	}
	Serial.print(f32, 5);
	Serial.print(symbol);
	Serial.println(f32r, 5);
	Serial.print("Roundtrip ");
	Serial.println(result);
	Serial.println();

	const size_t len = 40;
	char buf[len];
	struct efloat32_fields fields;
	efloat32_radix_2_to_fields(f32, &fields);
	int written;
	char *expr = efloat32_fields_to_expression(fields, buf, len, &written);
	Serial.println(expr);
	Serial.println("=================================================");

	delay(2000);
}
