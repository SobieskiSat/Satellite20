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

SX1278 radio;
uint8_t sendBuffer[SX1278_MAX_PACKET];
uint8_t message_length;

bool newIRQ;

static void setup(void)
{
	setupPins();

	while (HAL_GPIO_ReadPin(BTN_USR_GPIO_Port, BTN_USR_Pin) == GPIO_PIN_SET);
	println("Hello world!!");	HAL_Delay(500);

	//if (bmp280_begin()) println("[BMP] joined the server!");
	//if (sd_begin()) println("[SD] joined the server!");
	//if (radio_begin()) println("[LoRa] joined the server!");
	//enableMotors() println("[MOT] joined the server!");
}

static void loop(void)
{

}

static void mot_up_down(void)
{
	uint8_t i;
	for (i = 0; i < 255; i++)
	{
		if (HAL_GPIO_ReadPin(BTN_USR_GPIO_Port, BTN_USR_Pin) == GPIO_PIN_RESET)
		{
			setMotors((float)i / 255, (float)i / 255);
			HAL_GPIO_WritePin(LEDD_GPIO_Port, LEDD_Pin, GPIO_PIN_SET);
		}
		else
		{
			haltMotors();
			HAL_GPIO_WritePin(LEDD_GPIO_Port, LEDD_Pin, GPIO_PIN_RESET);
		}

		HAL_Delay(10);
	}
	for (i = 255; i > 0; i--)
	{
		if (HAL_GPIO_ReadPin(BTN_USR_GPIO_Port, BTN_USR_Pin) == GPIO_PIN_RESET)
		{
			setMotors((float)i / 255, (float)i / 255);
			HAL_GPIO_WritePin(LEDD_GPIO_Port, LEDD_Pin, GPIO_PIN_SET);
		}
		else
		{
			haltMotors();
			HAL_GPIO_WritePin(LEDD_GPIO_Port, LEDD_Pin, GPIO_PIN_RESET);
		}

		HAL_Delay(10);
	}
}
static void radio_receive(void)
{
	uint8_t i;
	if (!radio.useDio0IRQ)
	{
		if (SX1278_receive(&radio))
		{
			HAL_GPIO_TogglePin(LEDA_GPIO_Port, LEDA_Pin);
			printLen = sprintf(printBuffer, "[LoRa] Valid packet received! Bytes: %d, Rssi: %d, Data:\n\r", radio.rxLen, radio.rssi);
			printv(printBuffer, printLen);

			println("");
			printLen = sprintf(printBuffer, "%d\t%d\r\n", radio.rxBuffer[0], radio.rxBuffer[1]);
			printv(printBuffer, printLen);
			println("");

			for (i = 0; i < radio.rxLen; i++)
			{
				char character[1] = {0};
				character[0] = (char)radio.rxBuffer[i];
				print(character);
			}
			println("");

			// drive motors with values received from radio
			setMotors((float)radio.rxBuffer[0] / 255, (float)radio.rxBuffer[1] / 255);

			radio.newPacket = false;
		}
	}
}

static void dio0_IRQ(void)
{
	if (radio.pendingIRQ)
	{
		println("[LoRa] DIO0 interrupt received.");
		newIRQ = true;
		//SX1278_dio0_IRQ(&radio);
		//radio_procedure();
	}
	else
	{
		println("[LoRa] DIO0 interrupt received but NOT used!");
	}
}

static bool bmp280_begin(void)
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
static bool radio_begin(void)
{
	println("[LoRa] Joining the server...");
	radio.reset = LR_RESET_Pin;
	radio.dio0 = LR_DIO0_Pin;
	radio.nss = LR_NSS_Pin;
	radio.reset_port = LR_RESET_GPIO_Port;
	radio.dio0_port = LR_DIO0_GPIO_Port;
	radio.nss_port = LR_NSS_GPIO_Port;
	radio.spi = Get_SPI1_Instance();

	radio.config = sx1278_default_config;

	//radio.useDio0IRQ = true; println("[LoRa] I am using DIO0 interrupt.");
	radio.useDio0IRQ = false; println("[LoRa] I am not using DIO0 interrupt.");

	uint8_t attempts = 0;

	while (!SX1278_init(&radio))
	{
		println("Player [LoRa] could not join the server!");
		attempts++;
		if (attempts >= 1000)
		{
			println("[LoRa] Too many attempts, aborting...");
			return false;
		}
	}

	radio.txDone = true;
	radio.rxDone = true;
	radio.newPacket = false;
	newIRQ = false;

	return true;
}
static bool sd_begin(void)
{
	HAL_Delay(1000);
	println("[SD] Joining the server...");
	HAL_Delay(1000);
	if (SD_init() == FR_OK)
	{
		println("[SD] joined the server.");
		print("[SD] Creating test file...");
		if (SD_newFile("/test.txt") == FR_OK)
		{
			println("successful.");
			bool status = true;
			status |= (SD_writeToFile("text.txt", "Created on: ") == FR_OK);

			DateTime fileCreated = getTime();
			char dateStr[26] = {0};
			sprintf(dateStr, "%d-%d-20%d %d:%d:%d:%d", fileCreated.dayM, fileCreated.month, fileCreated.year,
													   fileCreated.hour, fileCreated.minute, fileCreated.second, fileCreated.msecond);

			status |= (SD_writeToFile("text.txt", dateStr) == FR_OK);

			if (status) println("[SD] Content writing successful!");
			else println("[SD] Content writing unsuccessful!");
		}
		else println("unsuccessful :(");
	}
	else println("Player [SD] could not join the server!");

	return true;
}

static void setupPins(void)
{
	HAL_GPIO_WritePin(LEDA_GPIO_Port, LEDA_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LEDC_GPIO_Port, LEDC_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LEDD_GPIO_Port, LEDD_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PH_L_GPIO_Port, PH_L_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PH_R_GPIO_Port, PH_R_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EN_L_GPIO_Port, EN_L_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EN_R_GPIO_Port, EN_R_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(P1_GPIO_Port, P1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(P2_GPIO_Port, P2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(P3_GPIO_Port, P3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(P4_GPIO_Port, P4_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(P5_GPIO_Port, P5_Pin, GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(P6_GPIO_Port, P6_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(P7_GPIO_Port, P7_Pin, GPIO_PIN_RESET);
}
