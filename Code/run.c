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

#include "motors.h"
#include "Peripherials/imuTest.c"

#include "Algorithms/algoGalgo.c"

#include "Peripherials/sdTest.c"

uint32_t lastMotUpdate;

static void setup(void)
{
	// wait for USR button press
	while (HAL_GPIO_ReadPin(BTN_USR_GPIO_Port, BTN_USR_Pin) == GPIO_PIN_SET);
	// begin the program
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_SET);
	println("Hello world!!");	HAL_Delay(500);
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_RESET);

	if (sdTest_begin()) println("[SD] MicroSD is working!");

	//if (imuTest_begin()) println("[IMU] Init successful!");

	//enableMotors(); println("[MOT] Motors enabled!");
	//lastMotUpdate = millis();
}

static void loop(void)
{
	/*
	imuTest_getData();		// get data from IMU
	imuTest_quatUpdate();	// compute data received

	if (millis() - lastMotUpdate >= 100)	// every 100ms get Euler angles and run motor alogrithm
	{
		imuTest_getEuler();
		algoGalgo(yaw);
		lastMotUpdate = millis();
	}
	*/
}
