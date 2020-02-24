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

#include "transmitter.c"

uint8_t message[256];
uint8_t message_len;

static void setup(void)
{
	// wait for USR button press
	while (HAL_GPIO_ReadPin(BTN_USR_GPIO_Port, BTN_USR_Pin) == GPIO_PIN_SET);
	// begin the program
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_SET);
	println("Hello world!!");	HAL_Delay(500);
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_RESET);

	if (transmitter_begin()) println("Radio init success!");

	message_len = sprintf(message, "Cats can have little a salami.");
}

static void loop(void)
{
	transmitter_loop(message, message_len);
}
