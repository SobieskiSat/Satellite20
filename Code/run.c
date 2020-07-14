#include "run.h"
#include "config.h"
#include <stdio.h>
#include <stdbool.h>
#include "main.h"
#include "stm32f4xx_hal.h"

#include "Scripts/loging.c"
#include "Scripts/duplex.c"
#include "Scripts/sensing.c"
#include "Scripts/steering.c"

/* To do:
- add .newData flag
- LoRa overcurrent setting
- SPS30
- change packetNumber limit on antenna
- termination watchdog
*/

static void setup(void)
{
	//writePin(LEDD, HIGH);	while (readPin(BTN_1) == HIGH);	writePin(LEDD, LOW);

	loging_setup();
	
	duplex_setup();
	
	sensing_setup();

	steering_setup();

	//writePin(LEDD, HIGH);	while (readPin(BTN_1) == HIGH);	writePin(LEDD, LOW);
}

static void loop(void)
{
	loging_loop();

	duplex_loop();

	// ADD: verify states

	sensing_loop();

	steering_loop();
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
