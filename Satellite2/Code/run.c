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
bool nextTX;
uint8_t message_length;

uint8_t i;
uint32_t lastMillis;

static void setup()
{
	HAL_GPIO_WritePin(LEDA_GPIO_Port, LEDA_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LEDC_GPIO_Port, LEDC_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LEDD_GPIO_Port, LEDD_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PH_L_GPIO_Port, PH_L_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PH_R_GPIO_Port, PH_R_Pin, GPIO_PIN_RESET);

	while (HAL_GPIO_ReadPin(BTN_USR_GPIO_Port, BTN_USR_Pin) == GPIO_PIN_SET)
	{
		print("Waiting for button press..");
		printLen = sprintf(printBuffer, "%d(<-should be ~100)\n\r", (int)(millis() - lastMillis));
		printv(printBuffer, printLen);	//should print: 100
		lastMillis = millis();
		HAL_Delay(100);
	}

	HAL_GPIO_WritePin(LEDA_GPIO_Port, LEDA_Pin, GPIO_PIN_SET);
	println("Hello world!!");
	HAL_Delay(500);
	HAL_GPIO_WritePin(LEDA_GPIO_Port, LEDA_Pin, GPIO_PIN_RESET);


	if (radio_begin())
	{
		println("[LoRa] joined the server!");
		//radio_procedure();	//starts radio loop, now radio wokrs dependend on DIO0 interrupt in ping-pong mode
	}


	/*
	sd_begin();
	 */

	/*
	println("[MOT] WATCH OUT NOW! THERE IS A CHANCE THAT PWM POLARITY IS FLIPPED!");
	println("[MOT] IN THIS CASE MOTORS WILL TURN ON AND WONT STOP!!!!");
	println("[MOT] Starting in 5 seconds!!");
	HAL_Delay(2000);
	print("[MOT] 3..");
	HAL_Delay(1000);
	print("2..");
	HAL_Delay(1000);
	print("1..");
	HAL_Delay(1000);
	println("0");

	motL = 0;
	motR = 0;
	enableMotors();
	setPwmFrequency(720);
	println("[MOT] Same frequency as in CanSatKit. Sound should be the same.");
	setMotorTimeout(1000);
	println("[MOT] Left motor: GPIO (copy on P7), Right motor: PWM (copy on P6)");
	*/

	nextTX = true;
	radio.txDone = true;

}

static void loop()
{
	for (i = 0; i < 4; i++)
	{
		if (!radio.useDio0IRQ)
		{
			HAL_Delay(500);
			radio_procedure();
		}

		if (radio.newPacket)
		{
			printLen = sprintf(printBuffer, "[LoRa] Valid packet received! Bytes: %d, Rssi: %d, Data:\n\r", radio.rxLen, radio.rssi);
			printv(printBuffer, printLen);

			for (i = radio.rxLen-1; i >= 0; i++)
			{
				char character[1] = {0};
				character[0] = (char)radio.rxBuffer[i];
				print(character);
			}
			println("");

			// drive motors with values received from radio
			//setMotors(radio.rxBuffer[0], radio.rxBuffer[1]);

			radio.newPacket = false;
		}
		else if (radio.txDone)
		{
			printLen = sprintf(printBuffer, "[LoRa] Packet sent: %s\r\n", sendBuffer);
			printv(printBuffer, radio.txLen + 23);
			radio.txDone = false;
		}
	}


/*

	if (HAL_GPIO_ReadPin(BTN_USR_GPIO_Port, BTN_USR_Pin) == GPIO_PIN_RESET)
	{
		HAL_GPIO_WritePin(EN_L_GPIO_Port, EN_L_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(EN_R_GPIO_Port, EN_R_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LEDA_GPIO_Port, LEDA_Pin, GPIO_PIN_SET);
		HAL_Delay(7000);
	}
	else
	{
		HAL_GPIO_WritePin(EN_L_GPIO_Port, EN_L_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(EN_R_GPIO_Port, EN_R_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LEDA_GPIO_Port, LEDA_Pin, GPIO_PIN_RESET);
	}

	//println("#######################");
	//println("[MOT] Motor test! Press USR.");
	//println("#######################");
	/*
	for (i = 0; i < 255; i++)
	{
		if (HAL_GPIO_ReadPin(BTN_USR_GPIO_Port, BTN_USR_Pin) == GPIO_PIN_RESET)
		{
			setMotors(i, i);
			HAL_GPIO_WritePin(EN_L_GPIO_Port, EN_L_Pin, (i % 5 == 0) ? GPIO_PIN_RESET : GPIO_PIN_SET);
			HAL_GPIO_WritePin(P7_GPIO_Port, P7_Pin, (i % 5 == 0) ? GPIO_PIN_RESET : GPIO_PIN_SET);
			HAL_GPIO_WritePin(LEDA_GPIO_Port, LEDA_Pin, GPIO_PIN_SET);
		}
		else
		{
			haltMotors();
			HAL_GPIO_WritePin(EN_L_GPIO_Port, EN_L_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(P7_GPIO_Port, P7_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LEDA_GPIO_Port, LEDA_Pin, GPIO_PIN_RESET);
		}

		HAL_Delay(100);
	}
	for (i = 255; i >= 0; i--)
	{
		if (HAL_GPIO_ReadPin(BTN_USR_GPIO_Port, BTN_USR_Pin) == GPIO_PIN_RESET)
		{
			setMotors(i, i);
			HAL_GPIO_WritePin(EN_L_GPIO_Port, EN_L_Pin, (i % 5 == 0) ? GPIO_PIN_RESET : GPIO_PIN_SET);
			HAL_GPIO_WritePin(P7_GPIO_Port, P7_Pin, (i % 5 == 0) ? GPIO_PIN_RESET : GPIO_PIN_SET);
			HAL_GPIO_WritePin(LEDA_GPIO_Port, LEDA_Pin, GPIO_PIN_SET);
		}
		else
		{
			haltMotors();
			HAL_GPIO_WritePin(EN_L_GPIO_Port, EN_L_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(P7_GPIO_Port, P7_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LEDA_GPIO_Port, LEDA_Pin, GPIO_PIN_RESET);
		}

		HAL_Delay(100);
	}
*/


}

static void radio_procedure()
{
	if (nextTX && false)
	{
		memset(sendBuffer, 0x00, SX1278_MAX_PACKET);
		message_length = sprintf(sendBuffer, "Cats can have little a salami.");
		SX1278_transmit(&radio, sendBuffer, message_length);

		nextTX = false;
	}
	else
	{
		SX1278_receive(&radio);

		nextTX = true;
	}
}

static bool bmp280_begin()
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


static bool radio_begin()
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

	return true;
}

static void dio0_IRQ()
{
	if (radio.pendingIRQ)
	{
		println("[LoRa] DIO0 interrupt received.");
		SX1278_dio0_IRQ(&radio);
		radio_procedure();
	}
	else
	{
		println("[LoRa] DIO0 interrupt received but NOT used!");
	}
}

static bool sd_begin()
{
	println("[SD] Joining the server...");
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
