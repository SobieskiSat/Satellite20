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
#include "clock.h"

#include "bmp280.h"
#include "mpu9250.h"
#include "gps.h"
#include "sx1278.h"
#include "sps30.h"

// ############ Run specific ############
static void setup(void);
static void loop(void);
static struct
{
	BMP280 bmp;
	MPU9250 mpu;
	GPS gps;
	SX1278 radio;
	SPS30 sps;
	float mot_l;
	float mot_r;
	bool motors_enabled;
	bool servo_enabled;
	float target_lat;
	float target_lon;
	float target_alt;
	float target_yaw;
	uint8_t operation_mode;
	void (*log_print)(const char* format, ...);
} Common;

// ############ GPIO writing ############
#define HIGH GPIO_PIN_SET
#define LOW GPIO_PIN_RESET
#define GPIO_Port(PIN) PIN ## _GPIO_Port
#define GPIO_Pin(PIN) PIN ## _Pin
#define writePin(PIN, VAL) HAL_GPIO_WritePin(GPIO_Port(PIN), GPIO_Pin(PIN), VAL)
#define togglePin(PIN) HAL_GPIO_TogglePin(GPIO_Port(PIN), GPIO_Pin(PIN))
#define readPin(PIN) HAL_GPIO_ReadPin(GPIO_Port(PIN), GPIO_Pin(PIN))

#define delay(MS) HAL_Delay(MS);

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

		uint32_t printStart = micros();
		uint32_t timeout = 10 * printLen + 70;
		while (CDC_Transmit_FS((uint8_t*) printBuffer, printLen) == USBD_BUSY)
		{
			if (micros() - printStart > timeout)
			{
				return false;
			}
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

		uint32_t printStart = micros();
		uint32_t timeout = 10 * printLen + 70;
		while (CDC_Transmit_FS((uint8_t*) printBuffer, printLen) == USBD_BUSY)
		{
			if (micros() - printStart > timeout)
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

static inline void printDate(void)
{
	DateTime now = getTime();
	println("%.2d-%.2d-20%.2d %.2d:%.2d:%.2d:.4%d",
			now.dayM, now.month, now.year,
			now.hour, now.minute, now.second, now.msecond);
}

// ############### Useful #################
static inline void floatToBytes(float* value, uint8_t* buffer) { for (uint8_t i = 0; i < 4; i++) buffer[i] = *((uint8_t*)(value) + i); }
static inline void bytesToFloat(uint8_t* buffer, float* value) { for (uint8_t i = 0; i < 4; i++) *((uint8_t*)(value) + i) = buffer[i]; }
static inline void bytesToFloatRev(uint8_t* buffer, float* value) { for (uint8_t i = 0; i < 4; i++) *((uint8_t*)(value) + 3 - i) = buffer[i]; }

#endif /* RUN_H_ */
