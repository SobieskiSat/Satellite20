// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//		Algorithm with PID feedback
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "motors.h"

float yaw_last_error;

static void algoGalgo(float yaw, float target_yaw)
{
	float maxPower = 0.8;	// scale of motor power
	float TurboMode= 50.0;
	// Aim center: yaw = 180*
//	float target_yaw = 180.0;
	float error=target_yaw-yaw;
	float thrust = 180.0+TurboMode;

	if (error<-180.0)
		error = target_yaw-yaw+360.0;
	else if (error>180.0)
		error = target_yaw-yaw-360.0;



//	print("target_yaw: "); print_float(target_yaw); println("");
//	print("yaw: "); print_float(yaw); println("");

//	print("error: "); print_float(error); println("");
	//print("yaw_last_error: "); print_float(yaw_last_error); println("");

	float kp=1.0; //dobrany
//	float ki=0.5;
	float kd=500.0; //dobrany 500 ok - 100ms

	float pid_p, pid_i, pid_d;

	pid_p=kp*error;
	pid_d=kd*((error-yaw_last_error)/10);
//	pid_i=pid_i+(ki*error);



//	print("pid_p: "); print_float(pid_p); println("");
//	print("pid_d: "); print_float(pid_d); println("");
//	print("pid_i: "); print_float(pid_i); println("");

	float PID;
//	if (error>-15 && error <15)
//		PID=pid_p+pid_d+pid_i;
//	else
		PID=pid_p+pid_d;

	if (PID>180.0-TurboMode)
			PID=180.0-TurboMode;
		else if (PID<-180.0+TurboMode)
			PID=-180.0+TurboMode;

//	print("PID: "); print_float(PID); println("");


	setMotors((thrust - PID) * maxPower * (1.0 / 360.0), (thrust + PID) * maxPower * (1.0 / 360.0) * 0.92);
//	setMotors(maxPower, maxPower*0.9); //prawie skalibrowane



//	setMotors((thrust - error) * maxPower * (1.0 / 360.0), (thrust + error) * maxPower * (1.0 / 360.0));

	// Shines purple LED if centered
	if (yaw >= target_yaw-2 && yaw <= target_yaw+2) HAL_GPIO_WritePin(LEDA_GPIO_Port, LEDA_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(LEDA_GPIO_Port, LEDA_Pin, GPIO_PIN_RESET);

	yaw_last_error=error;
}
