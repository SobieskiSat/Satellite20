// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// 		Steering code
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

#include "motors.h"
#include "Peripherials/imuTest.c"

#include "Algorithms/algoGalgo.c"

uint32_t lastMotUpdate;
uint32_t lastMotUpdate2;

float target_yaw;
time_t t;

static void steering_begin(void)
{
	if (imuTest_begin()) println("[IMU] Init successful!");

	enableMotors(); println("[MOT] Motors enabled!");
	lastMotUpdate = millis();
	lastMotUpdate2 = millis();

	yaw_last_error=0.0;
	target_yaw=180.0;

	srand((unsigned) time(&t));
}

static void steering_loop(void)
{
	imuTest_getData();		// get data from IMU
	imuTest_quatUpdate();	// compute data received


/*	if (millis() - lastMotUpdate2 >= 2000)	// every 10ms get Euler angles and run motor alogrithm
	{
		target_yaw=rand()%360; // losowo
		//target_yaw=fmod(target_yaw+1.0, 360);


		lastMotUpdate2 = millis();
	}
*/
	if (millis() - lastMotUpdate >= 10)	// every 10ms get Euler angles and run motor alogrithm
	{
		imuTest_getEuler();

		//algoGalgo(yaw, target_yaw);

		lastMotUpdate = millis();
	}

}
