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

#include "duplex.c"
#include "sensing.c"
#include "Peripherials/motorTest.c"
#include "Algorithms/algoGalgo.c"

uint32_t lastSave;
uint32_t lastMotUpdate;
float target_yaw;
float target_lat;
float target_lon;
uint8_t servoState;

static void setup(void)
{
	HAL_TIM_PWM_Start(Get_TIM3_Instance(), TIM_CHANNEL_3);
	TIM3->CCR3 = 990;

	// wait for USR button press
	//while (HAL_GPIO_ReadPin(BTN_USR_GPIO_Port, BTN_USR_Pin) == GPIO_PIN_SET);
	// begin the program
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_SET);
	println("Hello world!!");	HAL_Delay(500);
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_RESET);

	TIM3->CCR3 = 510;

	if (sdTest_begin()) { println("SD card is working!");}
	log_new();
	SD_init();
	if (duplex_begin()) { println("Radio is working");}

	sensing_begin();

	target_lat = 20.0;
	target_lon = 30.0;
	target_yaw = 180.0; //statyczne 180.0

	//while (HAL_GPIO_ReadPin(BTN_USR_GPIO_Port, BTN_USR_Pin) == GPIO_PIN_SET);

	HAL_GPIO_WritePin(LEDD_GPIO_Port, LEDD_Pin, GPIO_PIN_RESET);
	HAL_Delay(1000);

	duplex_loop("new transmit", 14, true);
	lastSave = millis();
	servoState = 0;
}

static void loop(void)
{
	sensing_loop();


	if (millis() - lastSave >= 500)	// save SD buffers while waiting for packet but no frequent than 1Hz
	{
		log_save();
		lastSave = millis();
	}



	if (duplex_checkINT())
	{
		radio.txLen = sprintf(radio.txBuffer, "%.01f_%.01f_%.07f_%.07f ", bmp.pressure, bmp.temperature, gps.latitudeDegrees, gps.longitudeDegrees); //imortant to leave last byte
		duplex_loop(radio.txBuffer, radio.txLen, true);

		/*
		if (isReceiving && (millis() - lastSave >= 1500))	// save SD buffers while waiting for packet but no frequent than 1Hz
		{
			println("Saving");
			log_save();
			println("Done saving");
			lastSave = millis();
		}
		*/


		if ((float)radio.rxBuffer[radio.rxLen - 1] * (360.0 / 255.0))
		{
			target_yaw = (float)radio.rxBuffer[radio.rxLen - 1] * (360.0 / 255.0);
		}

		print_float(target_yaw); println("<< YAW << YAW");
		print_int(radio.rxBuffer[0]); println("<< SERVO");
		if (radio.rxBuffer[0] == 1 || radio.rxBuffer[0] == 3) TIM3->CCR3 = 550;
		else TIM3->CCR3 = 990;
		if (radio.rxBuffer[0] == 2 || radio.rxBuffer[0] == 3) enableMotors();
		else disableMotors();
	}


	if (millis() - lastMotUpdate >= 10)	// every 10ms get Euler angles and run motor alogrithm
	{

		imuTest_getEuler();
		float brng = bearing(gps.latitudeDegrees, gps.longitudeDegrees, target_lat, target_lon);
	//	algoGalgo(yaw, brng); // target_yaw wyliczane z pozycji anteny;
		algoGalgo(yaw, target_yaw); //statyczny target_yaw
		//print_float(yaw); println("");
		lastMotUpdate = millis();
	}


}
