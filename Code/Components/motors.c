#include "motors.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include "config.h"
#include "run.h"

// Left motor: Timer3 Channel2
// Right motor: Timer3 Channel2
// PWM configured in Mode2 - Up-Down
// DRV8838 has max of 250kHz PWM frequency
// Motors will automatically turn off after (TIM4->ARR * 1ms) time, default 100ms
// Any new value applied will reset timeout timer

void setMotors(float dutyL, float dutyR)
{
	if (motorsEnabled)
	{
		TIM3->CCR2 = (uint32_t)((float)TIM3->ARR * (1 - dutyL));
		TIM3->CCR3 = (uint32_t)((float)TIM3->ARR * (1 - dutyR));
		TIM3->CNT = 0;

		TIM4->CNT = 0;	// reset timer counter -> clears motor timeout

		Common.mot_l = dutyL;
		Common.mot_r = dutyR;
	}
}

void haltMotors(void)
{
	// Function called on TIM4 overflow interrupt
	TIM3->CCR2 = TIM3->ARR;
	TIM3->CCR3 = TIM3->ARR;

	TIM4->CNT = 0;	// reset timer counter -> clears motor timeout

	Common.mot_l = 0;
	Common.mot_r = 0;
}

void enableMotors(void)
{
	if (!motorsEnabled)
	{
		haltMotors();
		motorsEnabled = true;

		//MAX_PWM_FREQ = 42000000 / (2 * PWM_RESOLUTION); // 82031 Hz for 256 resolution
		//setPwmFrequency(MAX_PWM_FREQ);

		writePin(PH_L, MOTOR_L_DIR);
		writePin(PH_R, MOTOR_R_DIR);

		HAL_TIM_PWM_Start(Get_TIM3_Instance(), TIM_CHANNEL_2);
		HAL_TIM_PWM_Start(Get_TIM3_Instance(), TIM_CHANNEL_3);

		haltMotors();

		writePin(MOT_STBY, LOW);
		delay(10);
		writePin(MOT_STBY, HIGH);
	}
}

void disableMotors(void)
{
	if (motorsEnabled)
	{
		writePin(MOT_STBY, LOW);
		haltMotors();
		motorsEnabled = false;
		HAL_TIM_PWM_Stop(Get_TIM3_Instance(), TIM_CHANNEL_2);
		HAL_TIM_PWM_Stop(Get_TIM3_Instance(), TIM_CHANNEL_3);
	}
}

void setPwmFrequency(uint32_t f_hz)
{
	// Frequency calculation for Mode2 PWM:
	// Period = 2 * ARR * Clock period
	// Clock period = PSC * Source period
	// <=>
	// ARR = Clock frequency / (2 * Frequency)
	// Clock frequency (42MHz) = Source frequency (42Mhz) / PSC (1)

	//TIM3->ARR = (uint32_t)(42000000 / TIM3->PSC) / (2 * f_hz);

	if (f_hz > MAX_PWM_FREQ)
	{
		//println("[MOT] PWM frequency too high! Setting maximum.");
		setPwmFrequency(MAX_PWM_FREQ);
	}
	else
	{
		uint32_t mehz = 42000000;
		uint32_t arro = mehz / f_hz;
		arro /= 2;
		TIM3->CNT = 0;
		TIM3->ARR = arro;
		//println("[MOT] Frequency set to: %luHz\n\r", f_hz);
	}
}

void setMotorTimeout(uint32_t timeout_ms)
{
	TIM4->CNT = 0;
	TIM4->ARR = timeout_ms;
	//println("[MOT] Timeout set to: %lums\n\r", timeout_ms);
}

void motorTimeout(void)
{
	/*
	haltMotors();
	HAL_GPIO_WritePin(LEDD_GPIO_Port, LEDD_Pin, GPIO_PIN_SET);
	println("[MOT] Motors halted due to timeout!");
	*/
}
