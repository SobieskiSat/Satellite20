#include <motors.h>
#include "stm32f4xx_hal.h"
#include "main.h"
#include "config.h"

// Left motor: Timer2 Channel2
// Right motor: Timer2 Channel4
// DRV8838 has max of 250kHz PWM frequency
// PWM frequency = 42MHz / (PSC+1) = 42MHz / 420 = 100kHz

static void setMotors(uint8_t pwmL, uint8_t pwmR)
{
	//							some value	 / [0.0:1.0] duty cycle
	TIM2->CCR2 = (uint32_t)((float)TIM2->ARR / ((float)pwmL / 255.0));
	TIM2->CCR4 = (uint32_t)((float)TIM2->ARR / ((float)pwmR / 255.0));
}

static void haltMotors()
{
	TIM2->CCR2 = 0;
	TIM2->CCR4 = 0;
}

static void enableMotors()
{
	haltMotors();
	motL_forward = true;
	motR_forward = true;

	// flip direction based on config
	HAL_GPIO_WritePin(PH_L_GPIO_Port, PH_L_Pin, motL_forward ^ MOTOR_L_DIR);
	HAL_GPIO_WritePin(PH_R_GPIO_Port, PH_R_Pin, motR_forward ^ MOTOR_R_DIR);

	HAL_TIM_PWM_Start(Get_TIM2_Instance(), TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(Get_TIM2_Instance(), TIM_CHANNEL_4);
}

static void disableMotors()
{
	haltMotors();
	HAL_TIM_PWM_Stop(Get_TIM2_Instance(), TIM_CHANNEL_2);
	HAL_TIM_PWM_Stop(Get_TIM2_Instance(), TIM_CHANNEL_4);
}

static void setPwmFrequency(uint32_t f_hz)
{
	//TIM2->ARR = (uint32_t)(42000000 / TIM2->PSC);
	TIM2->ARR = 100000 / f_hz;
}
