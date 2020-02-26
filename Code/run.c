// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// 		Main file of the user code
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#include "run.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#include "stm32f4xx_hal_gpio.h"
#include <math.h>
#include <stdlib.h>

#include "Peripherials/sdTest.c"
#include "logger.h"

#include "transmitter.c"
//#include "receiver.c"
#include "sensing.c"
#include "Peripherials/motorTest.c"
#include "Algorithms/algoGalgo.c"

uint32_t lastSave;
uint32_t lastMotUpdate;
float target_yaw;

static void setup(void)
{
	// wait for USR button press
	while (HAL_GPIO_ReadPin(BTN_USR_GPIO_Port, BTN_USR_Pin) == GPIO_PIN_SET);
	// begin the program
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_SET);
	println("Hello world!!");	HAL_Delay(500);
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_RESET);


	if (sdTest_begin()) println("SD card is working!");
	log_new();
	SD_init();

	if (transmitter_begin()) println("Radio is working!");
	//if (receiver_begin()) println("Radio is working!");

	sensing_begin();

	target_yaw = 180;
	while (HAL_GPIO_ReadPin(BTN_USR_GPIO_Port, BTN_USR_Pin) == GPIO_PIN_SET);
	enableMotors(); println("[MOT] Motors enabled!");

	transmitter_loop("new transmit", 14);
}

static void loop(void)
{
	if (millis() - lastSave >= 1000)
	{
		log_save();
		lastSave = millis();
	}

	sensing_loop();

	if (HAL_GPIO_ReadPin(radio.dio0_port, radio.dio0) == GPIO_PIN_SET)
	{
		radio.txLen = sprintf(radio.lastPacket, "%f_%f_%f", yaw, pitch, roll);
		if (transmitter_loop(radio.lastPacket, radio.txLen))
		{
			log_radio(&radio, true);
		}
	}

	if (millis() - lastMotUpdate >= 10)	// every 10ms get Euler angles and run motor alogrithm
	{
		imuTest_getEuler();
		algoGalgo(yaw, target_yaw);
		print_float(yaw); println("");
		lastMotUpdate = millis();
	}

}
