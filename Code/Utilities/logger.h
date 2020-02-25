#ifndef UTILITIES_LOGGER_H_
#define UTILITIES_LOGGER_H_

#include "sd.h"

#include "bmp280.h"
#include "gps.h"
#include "mpu9250.h"
#include "motors.h"
#include "sx1278.h"

// Memory usage: 6 * 1kB = 6kB

void log_new();
char logBuffer[1024];
uint16_t logBufferIndex;
void log_print(char* line);

char bmpBuffer[1024];
uint16_t gpsBufferIndex;
void log_bmp(BMP280 bmp);

char gpsBuffer[1024];
uint16_t gpsBufferIndex;
void log_gps(GPS gps);

char imuBuffer[1024];
uint16_t imuBufferIndex;
void log_imu(float* quat);

char motBuffer[1024];
uint16_t motBufferIndex;
void log_mot(float left, float right);

char radioBuffer[1024];
uint16_t radioBufferIndex;
void log_radio(SX1278 radio);

#endif /* UTILITIES_LOGGER_H_ */
