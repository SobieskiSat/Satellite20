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
#include "Scripts/animations.c"
#include "Components/motors.h"

/* To do:
- setup info packet
- LoRa overcurrent setting
*/

uint16_t flight_safety;
bool armsOpen, setupSuccess;
uint8_t terminate;

static void latchArms(void) { if (armsOpen)  { TIM5->CCR4 = 00; armsOpen = false; }}
static void openArms(void)  { if (!armsOpen) { TIM5->CCR4 = 1000; armsOpen = true; }}

static void terminator(void)
{
	terminate = 0;
	terminate |= ((Common.bmp.active) && (abs(Common.bmp.alt_dx) > TERMINAL_ALT_DX)) << 1; // Falling too fast
	terminate |= (abs(Common.mpu.yaw_dx) > TERMINAL_YAW_DX) << 2;	// Spinning too fast
	terminate |= ((abs(Common.mpu.pitch - 90) > TERMINAL_HOR) ||
				  (abs(Common.mpu.roll - 90) > TERMINAL_HOR)) << 3;		// In vertical position
	terminate |= ((abs(Common.gps.latitudeDegrees - Common.target_lat) < KEEPOUT_LAT) &&
				  (abs(Common.gps.longitudeDegrees - Common.target_lon) < KEEPOUT_LON)) << 4;	// At the destination (planar)
	//terminate |= (Common.bmp.altitude - Common.target_alt < KEEPOUT_ALT) << 5;	// Close to the ground
	terminate |= (Common.gps.fix && ((Common.gps.altitude - Common.target_alt) < KEEPOUT_ALT)) << 5;	// Close to the ground

	if (Common.operation_mode != 31)
	{
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
			if (Common.operation_mode != 3)
			{
				Common.servo_enabled = true;
				Common.motors_enabled = true;
			}

			if (Common.servo_enabled) openArms();
			if (Common.motors_enabled) enableMotors();
		}
		else flight_safety++;

	}
	else
	{
		if (terminate == 0) Common.operation_mode = 0;
		else
		{
			Common.servo_enabled = false;
			Common.motors_enabled = false;
			latchArms();
			disableMotors();
		}
	}
}

static void setup(void)
{
/*
	char co[2] = {0, '\0'};
	Common.gps.uart = Get_UART1_Instance();
	while (true)
	{
		co[0] = '.';
		HAL_UART_Receive(Common.gps.uart, Common.gps.uartBuffer, 1, 500);
		co[0] = Common.gps.uartBuffer[0];
		print(co);
	}
*/

	setupSuccess = true;
	animation_dir = 1;

	#if INTERFACE_BTN
		while (readPin(BTN_1) == LOW) { leds_bounce(); delay(1); }
	#endif
	play_animation(&leds_fill, 2000);
	leds_low();

	armsOpen = true;
	flight_safety = 0;
	latchArms();
	Common.servo_enabled = false;
	Common.motors_enabled = false;

	setupSuccess &= loging_setup();
	setupSuccess &= duplex_setup();
	setupSuccess &= sensing_setup();
	setupSuccess &= steering_setup();

	#if INTERFACE_BTN
		while (readPin(BTN_1) == LOW) { setupSuccess ? leds_confirm() : leds_negate(); delay(1); }
	#endif
	play_animation(&leds_fill, 2000);
	leds_low();

	Common.operation_mode = 0;
}

static void loop(void)
{
	sensing_loop();

	duplex_loop();

	writePin(LEDC, Common.motors_enabled);

	terminator();

	steering_loop();

	loging_loop();
}

/*
uint8_t i;
//enableMotors();
for (i = 0; i <= 254; i++)
{
	setMotors(((float)i) / 255.0, ((float)i) / 255.0);
	delay(100);
}
delay(2000);
print("Between");
//enableMotors();
delay(2000);
for (i = 254; i > 0; i--)
{
	setMotors(((float)i) / 255.0, ((float)i) / 255.0);
	delay(100);
}
delay(400);
*/
