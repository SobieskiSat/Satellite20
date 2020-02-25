#include "logger.h"
#include "sd.h"
#include "clock.h"
#include "bmp280.h"
#include "gps.h"
#include "mpu9250.h"
#include "motors.h"
#include "sx1278.h"
#include "run.h"

void log_new()
{
	DateTime now = getTime();

	char directoryName[8] = {0};
	//sprintf(directoryName, "/%02d%02d%02d%02", now.hour, now.minute, now.dayM, now.month);
	sprintf(directoryName, "/heilo");

	println("");
	print(directoryName);
	println("");
	print(strcat(directoryName, "/LOG.TXT"));
	println("");

	print_int(SD_init());
	//print_int(SD_newDirectory((char*)directoryName));
	print_int(f_mkdir("/heilo"));
	print_int(SD_newFile(strcat(directoryName, "/LOG.TXT")));
	print_int(SD_newFile(strcat(directoryName, "/BMP.TXT")));
	print_int(SD_newFile(strcat(directoryName, "/GPS.TXT")));
	print_int(SD_newFile(strcat(directoryName, "/IMU.TXT")));
	print_int(SD_newFile(strcat(directoryName, "/MOT.TXT")));
	print_int(SD_newFile(strcat(directoryName, "/RADIO.TXT")));
	print_int(SD_deinit());
}


void log_print(char* line)
{

}

void log_bmp(BMP280 bmp)
{

}
void log_gps(GPS gps)
{

}
void log_imu(float* quat)
{

}

void log_mot(float left, float right)
{

}
void log_radio(SX1278 radio)
{

}
