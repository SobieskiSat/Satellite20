#include "run.h"
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"

#include "bmp280.h"

BMP280 bmp280;
float pressure, temperature;

void setup()
{
	bmp280_get_default_config(&bmp280.params);
	bmp280.addr = BMP280_I2C_ADDRESS_0;
	bmp280.i2c = Get_I2C1_Instance();

	while (!bmp280_init(&bmp280, &bmp280.params)) {
		print("BMP280 initialization failed\n");

		HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
		HAL_Delay(500);
	}

	print("BMP280 found!");
	HAL_GPIO_TogglePin(LED_GRN_GPIO_Port, LED_GRN_Pin);
	HAL_Delay(500);
}

void loop()
{
	while (!bmp280_read_float(&bmp280, &temperature, &pressure))
	{
		print("Temperature/pressure reading failed\n");
		HAL_Delay(2000);
	}

	sprintf(printBuffer, "Pressure: %.2f Pa, Temperature: %.2f C\r\n", pressure, temperature);
	print(printBuffer);

	HAL_Delay(50);
}
