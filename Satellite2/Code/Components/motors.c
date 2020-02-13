#include "motors.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include "config.h"
#include "run.h"

// Left motor: Timer2 Channel2
// Right motor: Timer2 Channel4
// PWM configured in Mode2 - Up-Down
// DRV8838 has max of 250kHz PWM frequency
// Motors will automatically turn off after (TIM4->ARR * 1ms) time, default 100ms
// Any new value applied will reset timeout timer

static void setMotors(float dutyL, float dutyR)
{
	TIM5->CCR2 = (uint32_t)((float)TIM5->ARR * (1 - dutyL));
	TIM5->CCR4 = (uint32_t)((float)TIM5->ARR * (1 - dutyR));
	TIM5->CNT = 0;

	TIM4->CNT = 0;	// reset timer counter -> clears motor timeout

	//todelete (timeout marking)
	HAL_GPIO_WritePin(LEDD_GPIO_Port, LEDD_Pin, GPIO_PIN_RESET);
}

static void haltMotors()
{
	// Function called on TIM4 overflow interrupt
	TIM5->CCR2 = TIM5->ARR;
	TIM5->CCR4 = TIM5->ARR;

	TIM4->CNT = 0;	// reset timer counter -> clears motor timeout
}

static void enableMotors()
{
	haltMotors();

	//MAX_PWM_FREQ = 42000000 / (2 * PWM_RESOLUTION); // 82031 Hz for 256 resolution
	//setPwmFrequency(MAX_PWM_FREQ);

	motL_forward = true;
	motR_forward = true;

	// flip direction based on config
	HAL_GPIO_WritePin(PH_L_GPIO_Port, PH_L_Pin, motL_forward ^ MOTOR_L_DIR);
	HAL_GPIO_WritePin(PH_R_GPIO_Port, PH_R_Pin, motR_forward ^ MOTOR_R_DIR);

	HAL_TIM_PWM_Start(Get_TIM5_Instance(), TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(Get_TIM5_Instance(), TIM_CHANNEL_4);

	haltMotors();
	println("[MOT] MOTORS ENABLED!!");
}

static void disableMotors()
{
	haltMotors();
	HAL_TIM_PWM_Stop(Get_TIM5_Instance(), TIM_CHANNEL_2);
	HAL_TIM_PWM_Stop(Get_TIM5_Instance(), TIM_CHANNEL_4);
}

static void setPwmFrequency(uint32_t f_hz)
{
	// Frequency calculation for Mode2 PWM:
	// Period = 2 * ARR * Clock period
	// Clock period = PSC * Source period
	// <=>
	// ARR = Clock frequency / (2 * Frequency)
	// Clock frequency (42MHz) = Source frequency (42Mhz) / PSC (1)

	//TIM5->ARR = (uint32_t)(42000000 / TIM5->PSC) / (2 * f_hz);

	if (f_hz > MAX_PWM_FREQ)
	{
		println("[MOT] PWM frequency too high! Setting maximum.");
		setPwmFrequency(MAX_PWM_FREQ);
	}
	else
	{
		uint32_t mehz = 42000000;
		uint32_t arro = mehz / f_hz;
		arro /= 2;
		TIM5->CNT = 0;
		TIM5->ARR = arro;

		printLen = sprintf(printBuffer, "[MOT] Frequency set to: %luHz\n\r", f_hz);
		printv(printBuffer, printLen);
	}
}

static void setMotorTimeout(uint32_t timeout_ms)
{
	TIM4->CNT = 0;
	TIM4->ARR = timeout_ms;
	printLen = sprintf(printBuffer, "[MOT] Timeout set to: %lums\n\r", timeout_ms);
	printv(printBuffer, printLen);
}

static void motorTimeout()
{
	haltMotors();
	HAL_GPIO_WritePin(LEDD_GPIO_Port, LEDD_Pin, GPIO_PIN_SET);
	println("[MOT] Motors halted due to timeout!");
}
