#include "run.h"
#include "config.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "main.h"
#include "stm32f4xx_hal.h"

#include "Scripts/loging.c"
#include "Scripts/duplex.c"
#include "Scripts/sensing.c"
#include "Scripts/steering.c"
#include "Components/motors.h"

#include "sps30.h"

/* To do:
- setup info packet
- LoRa overcurrent setting
- SPS30
*/

uint8_t flight_safety;
bool armsOpen;

static void latchArms(void)
{
	if (armsOpen)
	{	
		//TIM_->CCR_ = 900;
		armsOpen = false;
	}
}

static void openArms(void)
{
	if (!armsOpen)
	{
		//TIM_->CCR_ = 550;
		armsOpen = true;
	}
}

static void terminator(void)
{
	if (Common.operation_mode != 31)
	{
		uint8_t terminate = 0;
		terminate |= ((Common.bmp.active) && (-(Common.bmp.alt_dx) > TERMINAL_ALT_DX)) << 1; // Falling too fast
		terminate |= (abs(Common.mpu.yaw_dx) > TERMINAL_YAW_DX) << 2;	// Spinning too fast 
		terminate |= ((abs(Common.mpu.pitch) > TERMINAL_HOR) ||
					  (abs(Common.mpu.roll) > TERMINAL_HOR)) << 3;		// In vertical position
		terminate |= ((abs(Common.gps.latitudeDegrees - Common.target_lat) < KEEPOUT_LAT) ||
					  (abs(Common.gps.longitudeDegrees - Common.target_lon) < KEEPOUT_LON)) << 4;	// At the destination (planar)
		terminate |= (Common.bmp.altitude - Common.target_alt < KEEPOUT_ALT) << 5;	// Close to the ground

		if (Common.operation_mode == 3) flight_safety = FLIGHT_START_THRE;
		else if (terminate > 0)
		{
			Common.operation_mode = 31;
			#if RUN_DEBUG
				println("warning: [RUN] Flight terminated! Code: 0x%x", terminate);
			#endif
			(*Common.log_print)("*T0x%x", terminate);
			flight_safety = 0;
			Common.servo_enabled = false;
			Common.motors_enabled = false;
			latchArms();
			disableMotors();
			return;
		}

		if (flight_safety >= FLIGHT_START_THRE)
		{
			if (Common.servo_enabled || Common.operation_mode != 3) { openArms(); Common.servo_enabled = true; }
			if (Common.motors_enabled || Common.operation_mode != 3) { enableMotors(); Common.motors_enabled = true; }
		}
		else flight_safety++;

	}
	else
	{
		latchArms();
		disableMotors();
	}
}

static void setup(void)
{
	//writePin(LEDD, HIGH);	while (readPin(BTN_1) == HIGH);	writePin(LEDD, LOW);

	delay(2000);

	armsOpen = true;
	flight_safety = 0;
	latchArms();
	Common.servo_enabled = false;
	Common.motors_enabled = false;

	loging_setup();
	
	duplex_setup();

	sensing_setup();

	steering_setup();

	//writePin(LEDD, HIGH);	while (readPin(BTN_1) == HIGH);	writePin(LEDD, LOW);
}

static void loop(void)
{
	sensing_loop();

	duplex_loop();

	terminator();

	steering_loop();

	loging_loop();
}
