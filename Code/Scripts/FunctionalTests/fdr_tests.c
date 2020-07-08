#include "run.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#include "stm32f4xx_hal_gpio.h"
#include <math.h>
#include <stdlib.h>

#include "Scripts/Peripherials/sdTest.c"
#include "logger.h"

#include "Scripts/duplex.c"
#include "Scripts/sensing.c"
#include "Scripts/Peripherials/motorTest.c"
#include "Scripts/Algorithms/algoGalgo.c"

uint32_t lastSave;
uint32_t lastMotUpdate;
float target_yaw;
float target_lat;
float target_lon;
uint8_t servoState;

static void fdr_setup(void)
{
	HAL_TIM_PWM_Start(Get_TIM3_Instance(), TIM_CHANNEL_3);
	TIM3->CCR3 = 990;

	// wait for USR button press
	//while (HAL_GPIO_ReadPin(BTN_USR_GPIO_Port, BTN_USR_Pin) == GPIO_PIN_SET);
	// begin the program
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_SET);
	println("Hello world!!");	HAL_Delay(500);
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_RESET);

	TIM3->CCR3 = 600;

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
	//HAL_Delay(1000);

	duplex_loop("new transmit", 14, true);
	lastSave = millis();
	servoState = 0;
}

static void preparePacket()
{
	uint32_t temv = 0;
	temv = (uint32_t)(bmp.pressure * 10);
	radio.txBuffer[0] = (uint8_t)(temv >> 0) & 0xFF;
	radio.txBuffer[1] = (uint8_t)(temv >> 8) & 0xFF;
	radio.txBuffer[2] = (uint8_t)(temv >> 16) & 0xFF;
	radio.txBuffer[3] = (uint8_t)(temv >> 24) & 0xFF;

	temv = (uint32_t)(bmp.temperature * 10);
	radio.txBuffer[4] = (uint8_t)(temv >> 0) & 0xFF;
	radio.txBuffer[5] = (uint8_t)(temv >> 8) & 0xFF;
	radio.txBuffer[6] = (uint8_t)(temv >> 16) & 0xFF;
	radio.txBuffer[7] = (uint8_t)(temv >> 24) & 0xFF;

	temv = (uint32_t)(gps.latitudeDegrees * 10000000);
	radio.txBuffer[8] = (uint8_t)(temv >> 0) & 0xFF;
	radio.txBuffer[9] = (uint8_t)(temv >> 8) & 0xFF;
	radio.txBuffer[10] = (uint8_t)(temv >> 16) & 0xFF;
	radio.txBuffer[11] = (uint8_t)(temv >> 24) & 0xFF;

	temv = (uint32_t)(gps.longitudeDegrees * 10000000);
	radio.txBuffer[12] = (uint8_t)(temv >> 0) & 0xFF;
	radio.txBuffer[13] = (uint8_t)(temv >> 8) & 0xFF;
	radio.txBuffer[14] = (uint8_t)(temv >> 16) & 0xFF;
	radio.txBuffer[15] = (uint8_t)(temv >> 24) & 0xFF;


	radio.txBuffer[16] = (uint8_t)(mpu.yaw * (255.0 / 360.0));
	radio.txBuffer[17] = (uint8_t)(mpu.pitch * (255.0 / 360.0));
	radio.txBuffer[18] = (uint8_t)(mpu.roll * (255.0 / 360.0));

	radio.txBuffer[19] = 0x00;
	radio.txLen = 20;
}


static void fdr_loop(void)
{
	sensing_loop();


	if (millis() - lastSave >= 1137)	// save SD buffers while waiting for packet but no frequent than 1Hz
	{
		log_print("yo helo!\n\r");
		log_save();
		lastSave = millis();
	}



	if (duplex_checkINT())
	{
		//radio.txLen = sprintf(radio.txBuffer, "%.01f_%.01f_%.07f_%.07f ", bmp.pressure, bmp.temperature, gps.latitudeDegrees, gps.longitudeDegrees); //imortant to leave last byte
		preparePacket();
		duplex_loop(radio.txBuffer, radio.txLen, true);

		if ((float)radio.rxBuffer[radio.rxLen - 1] * (360.0 / 255.0))
		{
			target_yaw = (float)radio.rxBuffer[radio.rxLen - 1] * (360.0 / 255.0);
		}

		//print_float(target_yaw); println("<< YAW << YAW");
		//print_int(radio.rxBuffer[0]); println("<< SERVO");
		if (radio.rxBuffer[0] == 1 || radio.rxBuffer[0] == 3) TIM3->CCR3 = 600;
		else TIM3->CCR3 = 900;
		if (radio.rxBuffer[0] == 2 || radio.rxBuffer[0] == 3) enableMotors();
		else disableMotors();
	}


	if (millis() - lastMotUpdate >= 10)	// every 10ms get Euler angles and run motor alogrithm
	{

		//imuTest_getEuler();
		float brng = bearing(gps.latitudeDegrees, gps.longitudeDegrees, target_lat, target_lon);
	    //algoGalgo(yaw, brng); // target_yaw wyliczane z pozycji anteny;

		algoGalgo(mpu.yaw, target_yaw); //statyczny target_yaw

		//print_float(yaw); println("");
		lastMotUpdate = millis();
	}
}
