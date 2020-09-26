/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* efloat_arduino.ino : a quick demo of libefloat usage in arduino*/
/* Copyright (C) 2018, 2019, 2020 Eric Herman */
/* https://github.com/ericherman/libefloat */

#include <Arduino.h>
#include <HardwareSerial.h>

#if defined( _VARIANT_ARDUINO_DUE_X_ ) || defined( ARDUINO_SAM_DUE )
#if ARDUINO_DUE_USB_PROGRAMMING == 1
#define SerialObj Serial
#else // default to the NATIVE port
#define SerialObj SerialUSB
#endif
#endif

#ifndef SerialObj
#define SerialObj Serial
#endif

#include "efloat.h"

/* globals */
uint32_t loop_count;

void setup(void)
{
	SerialObj.begin(9600);

	delay(50);

	SerialObj.println("Begin");

	loop_count = 0;
}

void loop(void)
{
	SerialObj.println("=================================================");
	++loop_count;

	SerialObj.print("sizeof(float) == ");
	SerialObj.println(sizeof(float));

	SerialObj.print("sizeof(double) == ");
	SerialObj.println(sizeof(double));

	SerialObj.print("sizeof(long double) == ");
	SerialObj.println(sizeof(long double));

	SerialObj.print("sizeof(efloat32) == ");
	SerialObj.println(sizeof(efloat32));

	SerialObj.print("sizeof(uint32_t) == ");
	SerialObj.println(sizeof(uint32_t));

	SerialObj.println();
	float denominator = (loop_count * 1.0);
	float f32 = (1.0 / denominator);

	SerialObj.print("f32 == ");
	SerialObj.println(f32);
	SerialObj.println();
	SerialObj.print("uint32_t u32 = efloat32_to_uint32_bits(");
	SerialObj.print(f32);
	SerialObj.println(")");

	uint32_t u32 = efloat32_to_uint32_bits(f32);
	SerialObj.print("u32 == ");
	SerialObj.println(u32);

	SerialObj.println();
	SerialObj.print("float f32r = uint32_bits_to_efloat32(");
	SerialObj.print(u32);
	SerialObj.println(")");
	float f32r = uint32_bits_to_efloat32(u32);
	SerialObj.print("f32r == ");
	SerialObj.println(f32r);

	SerialObj.println();
	SerialObj.print("Roundtrip ");
	const char *result;
	const char *symbol;
	if (f32 == f32r) {
		symbol = " == ";
		result = "(f32 == f32r) SUCCESS";
	} else {
		result = "(f32 != f32r) FAIL!";
		symbol = " != ";
	}
	SerialObj.print(f32);
	SerialObj.print(symbol);
	SerialObj.println(f32r);
	SerialObj.print("Roundtrip ");
	SerialObj.println(result);
	SerialObj.println();
	SerialObj.println("=================================================");

	delay(2000);
}
