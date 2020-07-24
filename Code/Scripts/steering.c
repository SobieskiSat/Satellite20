#ifndef STEERING_C
#define STEERING_C

#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#include "run.h"
#include "config.h"
#include "motors.h"
#include "clock.h"

uint32_t lastMotUpdate;
uint32_t lastYawUpdate;

uint32_t lastAlgo;

float yaw_last_error;

static void PID(float yaw, float target_yaw)
{
	float maxPower = 0.85;	// scale of motor power
	float TurboMode= 50.0;
	float error= target_yaw-yaw;
	float thrust;

	if (error<-180.0) //ulomne modulo
		error = target_yaw-yaw+360.0;
	else if (error>180.0)
		error = target_yaw-yaw-360.0;

	if (error>-10 && error<10)
		thrust = 340.0;
	else
		thrust = 180.0+TurboMode;

	float pid_p, pid_i, pid_d;

	pid_p=PID_kp*error;
	pid_d=PID_kd*((error-yaw_last_error)/(millis() - lastAlgo));
	//pid_i=pid_i+(PID_ki*error);

	float PID_coef;
	//if (error>-15 && error <15)
	//	PID_coef=pid_p+pid_d+pid_i;
	//else
		PID_coef=pid_p+pid_d;

	if (PID_coef>180.0-TurboMode)
			PID_coef=180.0-TurboMode;
		else if (PID_coef<-180.0+TurboMode)
			PID_coef=-180.0+TurboMode;

	setMotors(0.2 * ((thrust - PID_coef) * maxPower * (1.0 / 360.0)) + 0.6, 0.2 * ((thrust + PID_coef) * maxPower * (1.0 / 360.0)) + 0.6);

	//setMotors(maxPower, maxPower*0.9); //prawie skalibrowane
	//setMotors((thrust - error) * maxPower * (1.0 / 360.0), (thrust + error) * maxPower * (1.0 / 360.0));

	yaw_last_error=error;
	lastAlgo = millis();
}

static float bearing(float lat, float lon, float lat2, float lon2)
{
	//lat = your current gps latitude.
	//lon = your current gps longitude.
	//lat2 = your destiny gps latitude.
	//lon2 = your destiny gps longitude.

    float teta1 = lat*M_PI/180;
    float teta2 = lat2*M_PI/180;
    float delta1 = (lat2-lat)*M_PI/180;
    float delta2 = (lon2-lon)*M_PI/180;

    //==================Heading Formula Calculation================//

    float y = sin(delta2) * cos(teta2);
    float x = cos(teta1)*sin(teta2) - sin(teta1)*cos(teta2)*cos(delta2);
    float brng = atan2(y,x);
    brng = brng*180/M_PI;			// radians to degrees
    brng = fmod((brng + 360), 360);

    return brng;
}

static bool steering_setup(void)
{
	#if STEERING_ENABLE
		#if STEERING_DEBUG
			println("[STEERING] Activating PID algorithm");
		#endif
		(*Common.log_print)("*P00");

		yaw_last_error=0.0;
		Common.target_lat = DEFAULT_TARGET_LAT;
		Common.target_lon = DEFAULT_TARGET_LON;
		Common.target_alt = DEFAULT_TARGET_ALT;
		Common.target_yaw = DEFAULT_TARGET_YAW;
		motorsRunning = false;
		enableMotors();
		setMotors(0.9, 0.9);
		delay(200);
		setMotors(0, 0);
		disableMotors();
		if (!Common.mpu.active)
		{
			#if STEERING_DEBUG
				println("error: [STEERING] PID cannot function because IMU is not active!");
			#endif
			(*Common.log_print)("*EP00");
			return false;
		}
		return true;
	#else
		#if STEERING_DEBUG
			println("warning: [STEERING] PID IS DISABLED!")
		#endif
		(*Common.log_print)("*WP00");
		disableMotors();
		Common.motors_enabled = false;
		return false;
	#endif
}

static void steering_loop(void)
{
	#if STEERING_ENABLE
	if (Common.mpu.active && Common.operation_mode != 31 && Common.motors_enabled && Common.servo_enabled) 
	{
		if (millis() - lastMotUpdate >= STEERING_PID_DELAY)	// run motor alogrithm
		{	
			PID(Common.mpu.yaw, Common.target_yaw);
			lastMotUpdate = millis();
			writePin(LEDD, abs(Common.mpu.yaw - Common.target_yaw) < 3);
		}

		if ((Common.operation_mode == 0 || Common.operation_mode == 1) && millis() - lastYawUpdate >= STEERING_YAW_DELAY && Common.gps.fix)
		{
			Common.target_yaw = bearing(Common.gps.latitudeDegrees, Common.gps.longitudeDegrees, Common.target_lat, Common.target_lon); // target_yaw wyliczane z pozycji anteny
			lastYawUpdate = millis();
		}
	}
	#endif
}

#endif
