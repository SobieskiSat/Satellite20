#include "run.h"
#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"

#include "bmp280.h"
#include "config.h"

BMP280 bmp280;
float pressure, temperature;

SX1278 radio;


void setup()
{
	if (bmp280_begin())
	{
		HAL_GPIO_TogglePin(LED_GRN_GPIO_Port, LED_GRN_Pin);
		HAL_Delay(500);
	}

	if (radio_begin())
	{
		HAL_GPIO_TogglePin(LED_BLU_GPIO_Port, LED_BLU_Pin);
		HAL_Delay(500);
	}
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

bool bmp280_begin()
{
	bmp280.params = bmp280_default_config;
	bmp280.addr = BMP280_I2C_ADDRESS_0;
	bmp280.i2c = Get_I2C1_Instance();

	while (!bmp280_init(&bmp280, &bmp280.params)) {
		print("BMP280 initialization failed\n");

		HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
		HAL_Delay(500);
	}

	print("BMP280 found!");
}

bool radio_begin()
{
	radio.reset = LR_RESET_Pin;
	radio.dio0 = LR_DIO0_Pin;
	radio.nss = LR_NSS_Pin;
	radio.reset_port = LR_RESET_GPIO_Port;
	radio.dio0_port = LR_DIO0_GPIO_Port;
	radio.nss = LR_NSS_GPIO_Port;
	radio.spi = Get_SPI1_Instance();

	radio.frequency = (uint8_t*){(sx1278_default_config.frequency), NULL, NULL}; // to do: uint16_t => 3 * uint8_t
	radio.power = sx1278_default_config.power;
	radio.spreadingFactor = sx1278_default_config.spreadingFactor;
	radio.codingRate = sx1278_default_config.codingRate;
	radio.bandWidth = sx1278_default_config.bandWidth;
	radio.crc = sx1278_default_config.crc;

	SX1278_begin(&radio);

	if (master == 1)
	{
		ret = SX1278_LoRaEntryTx(&SX1278, 16, 2000);
	}
	else
	{
		ret = SX1278_LoRaEntryRx(&SX1278, 16, 2000);
	}
}
