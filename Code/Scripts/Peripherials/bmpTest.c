// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//		A code to test BMP280
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdbool.h>

#include "run.h"
#include "bmp280.h"
#include "clock.h"
#include "config.h"

BMP280 bmp280;
float temperature;
float pressure;

static bool bmpTest_begin(void)
{
	bmp280.params = bmp280_default_config;
	bmp280.i2c_addr = BMP280_I2C_ADDRESS;
	bmp280.i2c = Get_I2C2_Instance();

	while (!bmp280_init(&bmp280, &bmp280.params)) {
		print("BMP280 initialization failed\n");

		//HAL_GPIO_TogglePin(LEDC_GPIO_Port, LEDC_Pin);
		HAL_Delay(500);
		return false;
	}

	print("BMP280 found!\n");
	return true;
}


static void bmpTest_getData(void)
{
	/*
	while (!bmp280_read_float(&bmp280, &temperature, &pressure, &humidity))
	{
			size = sprintf((char *)Data,
					"Temperature/pressure reading failed\n\r");
			CDC_Transmit_FS(Data, size);
			HAL_Delay(2000);
	}

	size = sprintf((char *)Data,"Pressure: %.2f Pa, Temperature: %.2f C \n\r",
			pressure, temperature);
	CDC_Transmit_FS(Data, size);
	if (bme280p) {
		size = sprintf((char *)Data,", Humidity: %.2f\n", humidity);
		CDC_Transmit_FS(Data, size);
	}

	else {
		size = sprintf((char *)Data, "\n\r");
		CDC_Transmit_FS(Data, size);
	}
	*/
	bmp280_read_float(&bmp280, &temperature, &pressure);
}
