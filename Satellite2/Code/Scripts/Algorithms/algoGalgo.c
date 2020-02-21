// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//		Algorithm with Proportional feedback
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdbool.h>

#include "motors.h"

static void algoGalgo(float yaw)
{
	float maxPower = 0.8;	// scale of motor power
	// Aim center: yaw = 180*

	setMotors(yaw * maxPower * (1.0 / 360.0), (360.0 - yaw) * maxPower * (1.0 / 360.0));

	// Shines purple LED if centered
	if (yaw >= 178.0 && yaw <= 182.0) HAL_GPIO_WritePin(LEDA_GPIO_Port, LEDA_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(LEDA_GPIO_Port, LEDA_Pin, GPIO_PIN_RESET);
}
