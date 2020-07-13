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
#define MOTORTEST_ON_BTN_PRESS 0
#define power_treshold 250

static bool motorTest_begin(void)
{
	println("motorTest start!");
	enableMotors();
	return true;
}


static void mot_up_down(void)
{
	uint8_t i;
	//enableMotors();
	for (i = 0; i <= power_treshold; i++)
	{
		setMotors(((float)i) / 255.0, ((float)i) / 255.0);
		delay(100);
	}
	delay(2000);
	print("Between");
	//enableMotors();
	delay(2000);
	for (i = power_treshold; i > 0; i--)
	{
		setMotors(((float)i) / 255.0, ((float)i) / 255.0);
		delay(100);
	}
	delay(400);
	//disableMotors();
}
