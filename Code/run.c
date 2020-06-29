#include "run.h"
#include "config.h"
#include <stdio.h>
#include <stdbool.h>

#include "main.h"
#include "stm32f4xx_hal.h"


#include "gps.h"

/*
#include <string.h>
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define FILENAME file1
#define PASTER(x,y) x ## _ ## y
#define EVALUATOR(x,y)  PASTER(x,y)
#define setup EVALUATOR(FILENAME, setup)
*/

//GPS gps;

/*
 * GPRMC - fix, warning, lat, lon, speed, Course Made Good?, date of fix, magnetic variation
 * GPVTG - Track made good?, magnetic track made good?, speed, speed
 * GPGGA - time, lat, lon, fix, no. satellites, HDOP, alt, time since last DGPS update, DGPS reference station id
 * GPGSA - fix mode, fix, satellite ids used to fix, PDOP, HDOP, VDOP
 * GPGSV - no. messages, msg number, total no. satellites in view, [SV PRN number, elevation in deg, azimuth, signal strenght]
 * GPGLL - lat, lon, fix time, data status
 * GPTXT - custom message
 */


#include "Scripts/Peripherials/imuTest.c"
#include "Scripts/Peripherials/motorTest.c"


static void setup(void)
{
	//writePin(LEDD, HIGH);	while (readPin(BTN_USR) == HIGH);	writePin(LEDD, LOW);
	println("Hello world");
	

	//imuTest_begin();
	motorTest_begin();


	/*
	gps.uart = Get_UART3_Instance();
	GPS_init(&gps);
	 */
}

static void loop(void)
{
	//imuTest_loop();
	mot_up_down();

	/*
	if (HAL_UART_Receive(gps.uart, gps.uartBuffer, 1, HAL_MAX_DELAY) == HAL_OK)
	{
		char toPrint[2] = {gps.uartBuffer[0], '\0'};
		print(toPrint);
	}
	*/
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
