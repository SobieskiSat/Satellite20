#include "run.h"
#include "config.h"
#include <stdio.h>
#include <stdbool.h>

/*
#include <string.h>
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define FILENAME file1
#define PASTER(x,y) x ## _ ## y
#define EVALUATOR(x,y)  PASTER(x,y)
#define setup EVALUATOR(FILENAME, setup)
*/

#include "Scripts/Peripherials/imuTest.c"

static void setup(void)
{
	writePin(LEDD, HIGH);	while (readPin(BTN_USR) == HIGH);	writePin(LEDD, LOW);
	println("Hello world");
	
	imuTest_begin();
}

static void loop(void)
{
	imuTest_loop();
}

/* #### setup ####
	writePin(LEDD, HIGH);	while (readPin(BTN_USR) == HIGH);	writePin(LEDD, LOW);

	#if SD_ENABLE
		logger_setup();
	#endif
	#if RADIO_ENABLE
		duplex_setup();
	#endif
	#if (IMU_ENABLE || GPS_ENABLE || BMP_ENABLE)
		sensing_setup();
	#endif
	#if (MOTOR_ENABLE && IMU_ENABLE)
		steering_setup();
	#endif

	writePin(LEDD, HIGH);	while (readPin(BTN_USR) == HIGH);	writePin(LEDD, LOW);
*/

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
