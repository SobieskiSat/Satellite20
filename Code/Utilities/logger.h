#ifndef UTILITIES_LOGGER_H_
#define UTILITIES_LOGGER_H_

#include <stdbool.h>
#include "sd.h"

#include "bmp280.h"
#include "gps.h"
#include "mpu9250.h"
#include "motors.h"
#include "sx1278.h"

// Memory usage: 7 * 1kB = 7kB

char directoryName[32];
char directoryNameCopy[32];
char openedPath[32];
char timestamp[32];
char tempBuffer[4096];

void log_new();
char logBuffer[1024];
uint16_t logBufferIndex;
void log_print(char* line);

char bmpBuffer[1024];
uint16_t bmpBufferIndex;
void log_bmp(BMP280* bmp);

char gpsBuffer[1024];
uint16_t gpsBufferIndex;
void log_gps(GPS* gps);

char imuBuffer[1024];
uint16_t imuBufferIndex;
void log_imu(float* eulers);

char motBuffer[1024];
uint16_t motBufferIndex;
void log_mot(float left, float right);

char radioBuffer[4096];
uint16_t radioBufferIndex;
void log_radio(SX1278* radio, bool transmit);

void log_save();

#endif /* UTILITIES_LOGGER_H_ */
