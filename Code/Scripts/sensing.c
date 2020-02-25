// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//		All sensor code
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#include "run.h"
#include "config.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

#include "gps.h"
#include "bmp280.h"

#define SENSING_DEBUG 1

GPS gps;
BMP280 bmp;
// IMU
// (SPS)

static bool sensing_begin(void)
{
	uint8_t attempts = 0;
	gps.uart = Get_UART3_Instance();
	while (!GPS_init(&gps))
	{
		HAL_Delay(500);
		if (SENSING_DEBUG) println("[GPS] Init unsuccessful, retrying...");
		attempts++;

		if (attempts >= 5)
		{
			if (SENSING_DEBUG) println("[GPS] Too many attempts, GPS is not active!");
			break;
		}
	}

	attempts = 0;
	bmp.params = bmp280_default_config;
	bmp.addr = BMP280_I2C_ADDRESS_0;
	bmp.i2c = Get_I2C1_Instance();
	//unneccessary, to change
	while (!bmp280_init(&bmp, &bmp.params))
	{
		HAL_Delay(500);
		if (SENSING_DEBUG) println("[BMP] Init unsuccesfull, retrying...");
		attempts++;

		if (attempts >= 5)
		{
			if (SENSING_DEBUG) println("[BMP] Too many attempts, BMP is not active!");
			break;
		}
	}

}

static void sensing_loop(void)
{
	if (gps.active)
	{
		char c = GPS_read(&gps);
		if (GPS_newNMEAreceived(&gps))
		{
			GPS_parse(&gps, GPS_lastNMEA(&gps));
		}
	}
}
