#ifndef RUN_H_
#define RUN_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "usbd_cdc_if.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

// Set to 1 to print on USB, if not connected will wait for the computer!
#define PRINT_OUT 1

#define GPSECHO 0

static void setup(void);
static void loop(void);

static uint32_t printLen = 0;
static char printBuffer[512];
static inline void print(char* str)
{
	#if PRINT_OUT
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_SET);

	while (CDC_Transmit_FS((uint8_t*) str, strlen(str)) == USBD_BUSY);

	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_RESET);
	#endif
};
static inline void println(char* str)
{
	#if PRINT_OUT
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_SET);

	printLen = strlen(str);
	uint8_t i;
	for (i = 0; i < printLen; i++)
	{
		printBuffer[i] = str[i];
	}
	printBuffer[printLen] = '\n';
	printBuffer[printLen + 1] = '\r';
	printBuffer[printLen + 2] = '\0';
 	while (CDC_Transmit_FS((uint8_t*) printBuffer, printLen + 3) == USBD_BUSY);

	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_RESET);
	#endif
};
static inline void printv(char* str, uint32_t len)
{
	#if PRINT_OUT
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_SET);

	while (CDC_Transmit_FS((uint8_t*) str, len) == USBD_BUSY);

	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_RESET);
	#endif
}
static inline void print_int(int number)
{
	#if PRINT_OUT
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_SET);

	printLen = sprintf(printBuffer, "%d", number);
	while (CDC_Transmit_FS((uint8_t*) printBuffer, printLen) == USBD_BUSY);

	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_RESET);
	#endif
}
static inline void print_float(float number)
{
	#if PRINT_OUT
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_SET);

	printLen = sprintf(printBuffer, "%f", number);
	while (CDC_Transmit_FS((uint8_t*) printBuffer, printLen) == USBD_BUSY);

	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_RESET);
	#endif
}
static inline void print_char(char ch)
{
	#if PRINT_OUT
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_SET);

	char cha[1] = {ch};
	while (CDC_Transmit_FS((uint8_t*) cha, 1) == USBD_BUSY);

	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_RESET);
	#endif
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

#endif /* RUN_H_ */
