#include "run.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#include "stm32f4xx_hal_gpio.h"

#include "bmp280.h"
#include "sx1278.h"
#include "sd.h"
#include "motors.h"
#include "clock.h"
#include "config.h"

BMP280 bmp280;
float pressure, temperature;
uint8_t pressureBytes[4];
uint8_t temperatureBytes[4];

SX1278 radio;
uint8_t sendBuffer[SX1278_MAX_PACKET];
bool nextTX;
int message;
uint8_t message_length;

uint8_t umotL;
uint8_t umotR;
uint8_t counter;
int8_t counterDir;
bool notPlayed;
int i;

void setup()
{
	HAL_GPIO_WritePin(LEDA_GPIO_Port, LEDA_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LEDC_GPIO_Port, LEDC_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LEDD_GPIO_Port, LEDD_Pin, GPIO_PIN_RESET);
	HAL_Delay(750);

	HAL_GPIO_WritePin(LEDA_GPIO_Port, LEDA_Pin, GPIO_PIN_SET);
	HAL_Delay(500);
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_SET);
	HAL_Delay(500);
	HAL_GPIO_WritePin(LEDC_GPIO_Port, LEDC_Pin, GPIO_PIN_SET);
	HAL_Delay(500);
	HAL_GPIO_WritePin(LEDD_GPIO_Port, LEDD_Pin, GPIO_PIN_SET);
	HAL_Delay(500);

	motL = 0;
	motR = 0;
	counterDir = 1;
	enableMotors();
	setPwmFrequency(100000);
	notPlayed = true;

	/*
	if (bmp280_begin())
	{
		bmp280_read_float(&bmp280, &temperature, &pressure);
		println("BMP280 init successful!");
		HAL_GPIO_TogglePin(LEDA_GPIO_Port, LEDA_Pin);
		HAL_Delay(500);
	}

	if (radio_begin())
	{
		radio_procedure();	//starts radio loop, now radio wokrs dependend on DIO0 interrupt in ping-pong mode
		println("Radio init successful!");
		HAL_GPIO_TogglePin(LEDB_GPIO_Port, LEDB_Pin);
		HAL_Delay(500);
	}
	 */
}

void loop()
{
	if (!HAL_GPIO_ReadPin(BTN_USR_GPIO_Port, BTN_USR_Pin))
	{
		//Hearing frequency: 20Hz - 20kHz
		if (notPlayed)
		{
			notPlayed = false;
			for (i = 0; i < 100; i++)
			{
				setPwmFrequency(700);
				setMotors(80, 80);
				HAL_Delay(50);
			}
			for (i = 0; i < 100; i++)
			{
				setPwmFrequency(2000);
				setMotors(80, 80);
				HAL_Delay(50);
			}
			for (i = 0; i < 100; i++)
			{
				setPwmFrequency(1100);
				setMotors(80, 80);
				HAL_Delay(50);
			}
			for (i = 0; i < 100; i++)
			{
				setPwmFrequency(1700);
				setMotors(80, 80);
				HAL_Delay(50);
			}
			HAL_Delay(500);
			for (i = 0; i < 100; i++)
			{
				setPwmFrequency(1000);
				setMotors(80, 80);
				HAL_Delay(50);
			}
			for (i = 0; i < 100; i++)
			{
				setPwmFrequency(2400);
				setMotors(80, 80);
				HAL_Delay(50);
			}

		}

		HAL_GPIO_WritePin(LEDA_GPIO_Port, LEDA_Pin, GPIO_PIN_SET);
		counter += counterDir;
		if (counter >= 255 || counter <= 0)
		{
			counterDir = -counterDir;
		}
		umotL = counter;
		umotR = 255 - counter;
		setMotors(umotL, umotR);
		HAL_Delay(15);
	}
	else
	{
		HAL_GPIO_WritePin(LEDA_GPIO_Port, LEDA_Pin, GPIO_PIN_RESET);
		haltMotors();
	}

	/*
	if (bmp280_read_float(&bmp280, &temperature, &pressure))
	{
		printLen = sprintf(printBuffer, "Pressure: %.2f Pa, Temperature: %.2f C\r\n", pressure, temperature);
		printv(printBuffer, printLen);
		floatToBytes(pressure, pressureBytes);
		floatToBytes(temperature, temperatureBytes);
		//HAL_Delay(50);
	}

	if (radio.newPacket)
	{
		printLen = sprintf(printBuffer, "Received (%d @ %d):\t%s\r\n", radio.rxLen, radio.rssi, radio.rxBuffer);
		printv(printBuffer, printLen);

		// drive motors with values received from radio
		setMotors(radio.rxBuffer[0], radio.rxBuffer[1]);

		radio.newPacket = false;
	}
	else if (radio.txDone)
	{
		// will print whole 255 array, somehow assign message_length later
		printLen = sprintf(printBuffer, "Send: %s\r\n", sendBuffer);
		printv(printBuffer, printLen);
		message += 1;
		radio.txDone = false;
	}
	*/
}

void radio_procedure()
{
	if (nextTX)
	{
		memset(sendBuffer, 0x00, SX1278_MAX_PACKET);
		//message_length = sprintf(sendBuffer, "Hello %d", message);
		//SX1278_transmit(&radio, (uint8_t*)sendBuffer, message_length);

		// copy pressure and temperature data to buffer
		message_length = 8;
		sendBuffer[0] = pressureBytes[0];
		sendBuffer[1] = pressureBytes[1];
		sendBuffer[2] = pressureBytes[2];
		sendBuffer[3] = pressureBytes[3];
		sendBuffer[4] = temperatureBytes[0];
		sendBuffer[5] = temperatureBytes[1];
		sendBuffer[6] = temperatureBytes[2];
		sendBuffer[7] = temperatureBytes[3];

		SX1278_transmit(&radio, sendBuffer, message_length);

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


bool radio_begin()
{
	radio.reset = LR_RESET_Pin;
	radio.dio0 = LR_DIO0_Pin;
	radio.nss = LR_NSS_Pin;
	radio.reset_port = LR_RESET_GPIO_Port;
	radio.dio0_port = LR_DIO0_GPIO_Port;
	radio.nss_port = LR_NSS_GPIO_Port;
	radio.spi = Get_SPI1_Instance();

	radio.config = sx1278_default_config;

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

void dio0_IRQ()
{
	/*
	if (radio.pendingIRQ)
	{
		HAL_GPIO_TogglePin(LEDD_GPIO_Port, LEDD_Pin);
		SX1278_dio0_IRQ(&radio);
		radio_procedure();
	}
	*/
}

bool sd_begin()
{
	SD_init();

	HAL_GPIO_WritePin(LEDA_GPIO_Port, LEDA_Pin, GPIO_PIN_SET);
	println("SD init successful!");
	print("Creating test file...");
	if (SD_newFile("test.txt"))
	{
		HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_SET);
		println("successful");
		bool status = true;
		status |= SD_writeToFile("text.txt", "Created on: ");
		DateTime fileCreated = getTime();
		char dateStr[26] = {0};
		sprintf(dateStr, "%d-%d-20%d %d:%d:%d:%d", fileCreated.dayM, fileCreated.month, fileCreated.year,
												   fileCreated.hour, fileCreated.minute, fileCreated.second, fileCreated.msecond);
		status |= SD_writeToFile("text.txt", dateStr);
		if (status)
		{
			println("Content writing successful!");

			HAL_GPIO_WritePin(LEDC_GPIO_Port, LEDC_Pin, GPIO_PIN_SET);
		}
		else println("Content writing unsuccessful!");
	}
	else println("unsuccessful");

	return true;
}
