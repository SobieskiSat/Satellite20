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

//#include "transmitter.c"
//#include "receiver.c"
#include "sensing.c"
#include "Peripherials/motorTest.c"

uint32_t lastSave;

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

	//if (transmitter_begin()) println("Radio is working!");
	//if (receiver_begin()) println("Radio is working!");

	sensing_begin();

}

static void loop(void)
{
	if (millis() - lastSave >= 1000)
	{
		log_save();
		lastSave = millis();
	}

	sensing_loop();

}
