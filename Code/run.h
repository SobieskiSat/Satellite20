#ifndef RUN_H_
#define RUN_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

// Set to 1 to print on USB, if not connected will wait for the computer!
#define PRINT_OUT 1

// ############ Run routines ############
static void setup(void);
static void loop(void);

// ############ GPIO writing ############
#define HIGH GPIO_PIN_SET
#define LOW GPIO_PIN_RESET
#define GPIO_Port(PIN) PIN ## _GPIO_Port
#define GPIO_Pin(PIN) PIN ## _Pin
#define writePin(PIN, VAL) HAL_GPIO_WritePin(GPIO_Port(PIN), GPIO_Pin(PIN), VAL)
#define togglePin(PIN) HAL_GPIO_TogglePin(GPIO_Port(PIN), GPIO_Pin(PIN))
#define readPin(PIN) HAL_GPIO_ReadPin(GPIO_Port(PIN), GPIO_Pin(PIN))

// ############ USB printing ############
#define usbPresent hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED
static uint32_t printLen = 0;
static char printBuffer[512];

static inline bool print(const char* format, ...)
{
	if (usbPresent)
	{
		va_list argptr;
		va_start(argptr, format);
		printLen = vsprintf(printBuffer, format, argptr);
		va_end(argptr);

		uint8_t print_attempts = 0;
		while (CDC_Transmit_FS((uint8_t*) printBuffer, printLen) == USBD_BUSY)
		{
			if(print_attempts++ >= 10) return false;
		}
		return true;
	}
	return false;
}
static inline bool println(const char* format, ...)
{
	if (usbPresent)
	{
		va_list argptr;
		va_start(argptr, format);
		printLen = vsprintf(printBuffer, format, argptr);
		va_end(argptr);

		printBuffer[printLen] = '\r';
		printBuffer[printLen + 1] = '\n';
		printBuffer[printLen + 2] = '\0';
		printLen += 3;

		uint8_t print_attempts = 0;
		while (CDC_Transmit_FS((uint8_t*) printBuffer, printLen) == USBD_BUSY)
		{
			if(print_attempts++ >= 10) return false;
		}
		return true;
	}
	return false;
}


// ############ To delete ############
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
