#include "run.h"
#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"

#include "bmp280.h"
#include "sx1278.h"
#include "config.h"

BMP280 bmp280;
float pressure, temperature;

SX1278 radio;
uint8_t sendBuffer[SX1278_MAX_PACKET];
bool nextTX;
int message;
int message_length;

void setup()
{
	if (bmp280_begin())
	{
		bmp280_read_float(&bmp280, &temperature, &pressure);
		HAL_GPIO_TogglePin(LEDA_GPIO_Port, LEDA_Pin);
		HAL_Delay(500);
	}

	if (radio_begin())
	{
		radio_procedure();	//starts radio loop, now radio wokrs dependend on DIO0 interrupt in ping-pong mode
		HAL_GPIO_TogglePin(LEDB_GPIO_Port, LEDB_Pin);
		HAL_Delay(500);
	}
}

void loop()
{
	if (bmp280_read_float(&bmp280, &temperature, &pressure))
	{
		sprintf(printBuffer, "Pressure: %.2f Pa, Temperature: %.2f C\r\n", pressure, temperature);
		print(printBuffer);
		//HAL_Delay(50);
	}
	if (radio.newPacket)
	{
		printf("Received (%d @ %d):\t%s\r\n", radio.rxLen, radio.rssi, radio.rxBuffer);
		radio.newPacket = false;
	}
	else if (radio.txDone)
	{
		printf("Send: %s\r\n", sendBuffer);
		message += 1;
		radio.txDone = false;
	}
}

void radio_procedure()
{
	if (nextTX)
	{
		message_length = sprintf(sendBuffer, "Hello %d", message);
		SX1278_transmit(&radio, (uint8_t*)sendBuffer, message_length);
		print("Sending...\n");
		nextTX = false;
	}
	else
	{
		SX1278_receive(&radio);
		print("Receiving...\n");
		nextTX = true;
	}
}

bool bmp280_begin()
{
	bmp280.params = bmp280_default_config;
	bmp280.addr = BMP280_I2C_ADDRESS_0;
	bmp280.i2c = Get_I2C1_Instance();

	while (!bmp280_init(&bmp280, &bmp280.params)) {
		print("BMP280 initialization failed\n");

		HAL_GPIO_TogglePin(LEDC_GPIO_Port, LEDC_Pin);
		HAL_Delay(500);
		return false;
	}

	print("BMP280 found!\n");
	return true;
}


void dio0_IRQ()
{
	SX1278_dio0_IRQ(&radio);
	radio_procedure();
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

	// to change
	radio.frequency = sx1278_default_config.frequency;
	radio.power = sx1278_default_config.power;
	radio.spreadingFactor = sx1278_default_config.spreadingFactor;
	radio.codingRate = sx1278_default_config.codingRate;
	radio.bandWidth = sx1278_default_config.bandWidth;
	radio.crc = sx1278_default_config.crc;

	radio.useDio0IRQ = true;

	while (!SX1278_init(&radio))
	{
		print("SX1278 initialization failed\n");

		HAL_GPIO_TogglePin(LEDC_GPIO_Port, LEDC_Pin);
		HAL_Delay(500);

		return false;
	}

	return true;
}
