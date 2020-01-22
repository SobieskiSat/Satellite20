#include <stdio.h>
#include <string.h>
#include "main.h"
#include "run.h"
#include "fatfs.h"
#include "usb_device.h"
#include "stm32f4xx_hal.h"

static int counte = 0;
static char buf[20];

void setup()
{
	sprintf("Hello world!", 0);
	LED_animation();
}

void loop()
{
	HAL_GPIO_TogglePin(LED_BLU_GPIO_Port, LED_BLU_Pin);
	HAL_Delay(10);
	sprintf((char*)buf, "Counter: %d", counte);
}

void LED_animation()
{
	HAL_Delay(100);
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(LED_GRN_GPIO_Port, LED_GRN_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(LED_YEL_GPIO_Port, LED_YEL_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(LED_BLU_GPIO_Port, LED_BLU_Pin, GPIO_PIN_SET);

	HAL_Delay(100);
	HAL_GPIO_WritePin(LED_BLU_GPIO_Port, LED_BLU_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(LED_YEL_GPIO_Port, LED_YEL_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(LED_GRN_GPIO_Port, LED_GRN_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);
}
