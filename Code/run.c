#include "run.h"
#include "config.h"
#include <stdio.h>
#include <stdbool.h>
#include "main.h"
#include "stm32f4xx_hal.h"

#include "Scripts/loging.c"

/* To do:
- add .newData flag
- LoRa overcurrent setting
- SPS30
*/

static void setup(void)
{
	writePin(LEDD, HIGH);	while (readPin(BTN_1) == HIGH);	writePin(LEDD, LOW);

	loging_setup();
	#if RADIO_ENABLE
		duplex_setup();
	#endif
	#if (IMU_ENABLE || GPS_ENABLE || BMP_ENABLE)
		sensing_setup();
	#endif
	#if (MOTOR_ENABLE && IMU_ENABLE)
		steering_setup();
	#endif

	writePin(LEDD, HIGH);	while (readPin(BTN_1) == HIGH);	writePin(LEDD, LOW);
}

static void loop(void)
{
	loging_loop();
}

/* #### loop ####
	#if SD_ENABLE
		logger_loop();
	#endif
	#if RADIO_ENABLE
		duplex_loop(packet);
	#endif
	#if (IMU_ENABLE || GPS_ENABLE || BMP_ENABLE)
		sensing_loop();
	#endif
	#if (MOTOR_ENABLE && IMU_ENABLE)
		steering_loop(imu.yaw);
	#endif
*/
