// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//		A code to test motors
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdbool.h>

#include "motors.h"
#include "run.h"

// Set to 1 to drive motors only when USR is pressed
#define MOTORTEST_ON_USR_PRESS 0

static bool motorTest_begin(void)
{
	println("motorTest start!");
	enableMotors();
	return true;
}


static void mot_up_down(void)
{
	uint8_t i;
	for (i = 0; i < 254; i++)
	{
		if (HAL_GPIO_ReadPin(BTN_USR_GPIO_Port, BTN_USR_Pin) == (GPIO_PIN_RESET & MOTORTEST_ON_USR_PRESS))
		{
			setMotors(((float)i) / 255.0, ((float)i) / 255.0);
			HAL_GPIO_WritePin(LEDD_GPIO_Port, LEDD_Pin, GPIO_PIN_SET);
		}
		else
		{
			haltMotors();
			HAL_GPIO_WritePin(LEDD_GPIO_Port, LEDD_Pin, GPIO_PIN_RESET);
		}
		HAL_Delay(100);
	}
	HAL_Delay(400);
	for (i = 254; i > 0; i--)
	{
		if (HAL_GPIO_ReadPin(BTN_USR_GPIO_Port, BTN_USR_Pin) == (GPIO_PIN_RESET & MOTORTEST_ON_USR_PRESS))
		{
			setMotors(((float)i) / 255.0, ((float)i) / 255.0);
			HAL_GPIO_WritePin(LEDD_GPIO_Port, LEDD_Pin, GPIO_PIN_SET);
		}
		else
		{
			haltMotors();
			HAL_GPIO_WritePin(LEDD_GPIO_Port, LEDD_Pin, GPIO_PIN_RESET);
		}

		HAL_Delay(100);
	}
	HAL_Delay(400);
}
