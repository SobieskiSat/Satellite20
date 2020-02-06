#ifndef RUN_H_
#define RUN_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "usbd_cdc_if.h"

static void setup();
static void loop();

static uint32_t printLen = 0;
static char printBuffer[512];
static inline void print(char* str)
{
	CDC_Transmit_FS((uint8_t*) str, strlen(str));
};
static inline void println(char* str)
{
	printLen = strlen(str);
	uint8_t i;
	for (i = 0; i < printLen; i++)
	{
		printBuffer[i] = str[i];
	}
	printBuffer[printLen] = '\n';
	printBuffer[printLen] = '\r';
	CDC_Transmit_FS((uint8_t*) printBuffer, printLen + 2);
};
static inline void printv(char* str, uint32_t len)
{
	CDC_Transmit_FS((uint8_t*) str, len);
}

static inline void floatToBytes(float value, uint8_t bytes[4])
{
	union
	{
		float var;
		uint8_t arr[4];
	} uni;
	uni.var = value;
	memcpy(bytes, uni.arr, 4);
}

static bool bmp280_begin();
static bool radio_begin();
static void radio_procedure();
static bool sd_begin();

static void dio0_IRQ();

#endif /* RUN_H_ */
