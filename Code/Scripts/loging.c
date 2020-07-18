#ifndef LOGING_C
#define LOGING_C

#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "fatfs.h"

#include "run.h"
#include "config.h"

#include "sd.h"
#include "clock.h"

#include "bmp280.h"
#include "gps.h"
#include "mpu9250.h"
#include "motors.h"
#include "sx1278.h"

char directoryName[32];
char directoryNameCopy[32];
char openedPath[32];
char timestamp[32];
char tempBuffer[4096];

//void log_new();
char logBuffer[4096];
uint16_t logBufferIndex;
//void log_print(const char* format, ...);
static void log_print_dummy(const char* format, ...) { return; }

char bmpBuffer[1024];
uint16_t bmpBufferIndex;
//void log_bmp(BMP280* bmp);

char gpsBuffer[1024];
uint16_t gpsBufferIndex;
//void log_gps(GPS* gps);

char imuBuffer[1024];
uint16_t imuBufferIndex;
//void log_imu(MPU9250* mpu);

char motBuffer[1024];
uint16_t motBufferIndex;
//void log_mot(float left, float right);

char radioBuffer[4096];
uint16_t radioBufferIndex;
//void log_radio(SX1278* radio);

char spsBuffer[2048];
uint16_t spsBufferIndex;
//void log_sps(SPS30* sps);

uint32_t lastLogSave;
//void log_save();

uint32_t lastDataPrint;

bool sdActive;
uint32_t lastBmpLog;
uint32_t lastImuLog;
uint32_t lastMotLog;
uint32_t lastGpsLog;
uint32_t lastTargetYawLog;


static void log_new()
{
	DateTime now = getTime();
	sprintf(directoryName, "/%02d%02d%02d%02d", now.hour, now.minute, now.dayM, now.month);
	sprintf(directoryNameCopy, directoryName);

	SD_init();
	SD_newDirectory((char*)directoryName);
	SD_newFile(strcat(directoryName, "/LOG.TXT"));
	sprintf(directoryName, directoryNameCopy);
	SD_newFile(strcat(directoryName, "/BMP.TXT"));
	sprintf(directoryName, directoryNameCopy);
	SD_newFile(strcat(directoryName, "/GPS.TXT"));
	sprintf(directoryName, directoryNameCopy);
	SD_newFile(strcat(directoryName, "/IMU.TXT"));
	sprintf(directoryName, directoryNameCopy);
	SD_newFile(strcat(directoryName, "/MOT.TXT"));
	sprintf(directoryName, directoryNameCopy);
	SD_newFile(strcat(directoryName, "/RADIO.TXT"));
	sprintf(directoryName, directoryNameCopy);
	SD_newFile(strcat(directoryName, "/SPS.TXT"));
	sprintf(directoryName, directoryNameCopy);

	logBufferIndex = 0;
	bmpBufferIndex = 0;
	gpsBufferIndex = 0;
	imuBufferIndex = 0;
	motBufferIndex = 0;
	radioBufferIndex = 0;
	spsBufferIndex = 0;
}
static void log_save()
{
	lastLogSave = millis();
	//__disable_irq();
	if (logBufferIndex > 0)
	{
		sprintf(openedPath, strcat(directoryName, "/LOG.TXT"));
		sprintf(directoryName, directoryNameCopy);
		SD_writeToFile(openedPath, logBuffer);
	}

	if (bmpBufferIndex > 0)
	{
		sprintf(openedPath, strcat(directoryName, "/BMP.TXT"));
		sprintf(directoryName, directoryNameCopy);
		SD_writeToFile(openedPath, bmpBuffer);
	}

	if (gpsBufferIndex > 0)
	{
		sprintf(openedPath, strcat(directoryName, "/GPS.TXT"));
		sprintf(directoryName, directoryNameCopy);
		SD_writeToFile(openedPath, gpsBuffer);
	}

	if (imuBufferIndex > 0)
	{
		sprintf(openedPath, strcat(directoryName, "/IMU.TXT"));
		sprintf(directoryName, directoryNameCopy);
		SD_writeToFile(openedPath, imuBuffer);
	}

	if (motBufferIndex > 0)
	{
		sprintf(openedPath, strcat(directoryName, "/MOT.TXT"));
		sprintf(directoryName, directoryNameCopy);
		SD_writeToFile(openedPath, motBuffer);
	}

	if (radioBufferIndex > 0)
	{
		sprintf(openedPath, strcat(directoryName, "/RADIO.TXT"));
		sprintf(directoryName, directoryNameCopy);
		SD_writeToFile(openedPath, radioBuffer);
	}

	if (spsBufferIndex > 0)
	{
		sprintf(openedPath, strcat(directoryName, "/SPS.TXT"));
		sprintf(directoryName, directoryNameCopy);
		SD_writeToFile(openedPath, spsBuffer);
	}

	#if LOGING_DEBUG
		println("[LOGING] Buffers saved, took: %ums, ", millis() - lastLogSave);
		println("%d/4096 %d/1024 %d/1024 %d/1024 %d/1024 %d/4096 %d/2048", logBufferIndex, bmpBufferIndex, gpsBufferIndex, imuBufferIndex, motBufferIndex, radioBufferIndex, spsBufferIndex);
	#endif

	logBufferIndex = 0;
	bmpBufferIndex = 0;
	gpsBufferIndex = 0;
	imuBufferIndex = 0;
	motBufferIndex = 0;
	radioBufferIndex = 0;
	spsBufferIndex = 0;
	memset(logBuffer, 0x00, 4096);
	memset(bmpBuffer, 0x00, 1024);
	memset(gpsBuffer, 0x00, 1024);
	memset(imuBuffer, 0x00, 1024);
	memset(motBuffer, 0x00, 1024);
	memset(radioBuffer, 0x00, 4096);
	memset(spsBuffer, 0x00, 2048);
}

static void log_print(const char* format, ...)
{
	if (logBufferIndex < 3700)
	{
		sprintf(timestamp, "\t@%lu\r\n", millis());

		va_list argptr;
		va_start(argptr, format);
		vsprintf(tempBuffer, format, argptr);
		va_end(argptr);

		strcat(logBuffer, tempBuffer);
		strcat(logBuffer, timestamp);
		logBufferIndex = strlen(logBuffer);

		memset(tempBuffer, 0x00, 1024);
	}
}

static void log_bmp(BMP280* bmp)
{
	if (bmpBufferIndex < 950)
	{
		sprintf(timestamp, "\t@%lu\r\n", millis());
		sprintf(tempBuffer, "%.02f %.02f", bmp->pressure, bmp->temperature);
		strcat(tempBuffer, timestamp);
		strcat(bmpBuffer, tempBuffer);
		bmpBufferIndex = strlen(bmpBuffer);

		memset(tempBuffer, 0x00, 1024);
	}
}
static void log_gps(GPS* gps)
{
	if (gpsBufferIndex < 950)
	{
		sprintf(timestamp, "\t@%lu\r\n", millis());
		sprintf(tempBuffer, "%.07f %.07f %.01f %.2d-%.2d-20%.2d %.2d:%.2d:%.2d",
				gps->latitudeDegrees, gps->longitudeDegrees, gps->altitude,
				gps->gpsTime.dayM, gps->gpsTime.month, gps->gpsTime.year,
				gps->gpsTime.hour, gps->gpsTime.minute, gps->gpsTime.second);
		strcat(tempBuffer, timestamp);
		strcat(gpsBuffer, tempBuffer);
		gpsBufferIndex = strlen(gpsBuffer);

		memset(tempBuffer, 0x00, 1024);
	}
}
static void log_imu(MPU9250* mpu)
{
	if (imuBufferIndex < 950)
	{
		sprintf(timestamp, "\t@%lu\r\n", millis());
		sprintf(tempBuffer, "%.02f %.02f %.02f", mpu->yaw, mpu->pitch, mpu->roll);
		strcat(tempBuffer, timestamp);
		strcat(imuBuffer, tempBuffer);
		imuBufferIndex = strlen(imuBuffer);

		memset(tempBuffer, 0x00, 1024);
	}
}
static void log_mot(float left, float right)
{
	if (motBufferIndex < 950)
	{
		sprintf(timestamp, "\t@%lu\r\n", millis());
		sprintf(tempBuffer, "%.01f %.01f", left, right);
		strcat(tempBuffer, timestamp);
		strcat(motBuffer, tempBuffer);
		motBufferIndex = strlen(motBuffer);

		memset(tempBuffer, 0x00, 1024);
	}
}
static void log_radio(SX1278* radio)
{
	if (radioBufferIndex < 3700)
	{
		sprintf(timestamp, "]\t@%lu\r\n", millis());

		if (radio->newTxData)
		{
			sprintf(tempBuffer, "TX\t[");
			radio->txBuffer[radio->txLen] = '\0';
			strcat(tempBuffer, radio->txBuffer);
			radio->newTxData = false;
		}
		if (radio->newRxData)
		{
			sprintf(tempBuffer, "RX\t%d\t[", radio->rssi);
			radio->txBuffer[radio->rxLen] = '\0';
			strcat(tempBuffer, radio->rxBuffer);
			radio->newRxData = false;
		}

		strcat(tempBuffer, timestamp);
		strcat(radioBuffer, tempBuffer);
		radioBufferIndex = strlen(radioBuffer);

		memset(tempBuffer, 0x00, 1024);
	}
}
static void log_sps(SPS30* sps)
{
	if (spsBufferIndex < 1900)
	{
		sprintf(timestamp, "\t@%lu\r\n", millis());
		sprintf(tempBuffer, "%.1f %.1f %.1f %.1f | %.1f %.1f %.1f %.1f %.1f | %.1f",
							sps->pm1, sps->pm2, sps->pm4, sps->pm10,
							sps->n_pm05, sps->n_pm1, sps->n_pm2, sps->n_pm4, sps->n_pm10,
							sps->typical_size);
		strcat(tempBuffer, timestamp);
		strcat(spsBuffer, tempBuffer);
		spsBufferIndex = strlen(spsBuffer);

		memset(tempBuffer, 0x00, 1024);
	}
}


static bool loging_setup(void)		// Writes test file to SD card, if successful creates new log folder
{
	#if SD_ENABLE
		if (SD_init() == FR_OK)
		{
			#if LOGING_DEBUG
				println("[LOGING] SD detected, creating test file");
			#endif
			if (SD_newFile("/TEST.TXT") == FR_OK)
			{
				bool status = true;
				status &= (SD_writeToFile("/TEST.TXT", "Created on: ") == FR_OK);

				DateTime fileCreated = getTime();
				char dateStr[26] = {0};
				sprintf(dateStr, "%.2d-%.2d-20%.2d %.2d:%.2d:%.2d:%.4d",
						fileCreated.dayM, fileCreated.month, fileCreated.year,
						fileCreated.hour, fileCreated.minute, fileCreated.second, fileCreated.msecond);

				status &= (SD_writeToFile("/TEST.TXT", dateStr) == FR_OK);

				#if LOGING_DEBUG
					println("[LOGING] Content writing %s", status ? "ok" : "FAIL");
					if (!status) goto error_handler;
				#endif
			}
			else
			{
				#if LOGING_DEBUG
					println("[LOGING] Unable to create new file!");
				#endif
				goto error_handler;
			}
		}
		else
		{
			#if LOGING_DEBUG
				println("[LOGING] SD Init fail!");
			#endif
			goto error_handler;
		}

		sdActive = true;
		log_new();
		Common.log_print = &log_print;
		(*Common.log_print)("*L00"); // [LOGING] SD works, welcome onboard!
		return true;

		error_handler:
			#if LOGING_DEBUG
				println("error: [LOGING] SD logging is not active due to occured errors");
			#endif
			sdActive = false;
			Common.log_print = &log_print_dummy;
			return false;

	#else // SD_ENABLE
		println("warning: [LOGING] SD DISABLED!");
		Common.log_print = &log_print_dummy;
		return false;
	#endif
}

static void loging_loop(void)
{
	#if SD_ENABLE
		if (Common.bmp.active && millis() - lastBmpLog >= LOG_BMP_DELAY) { log_bmp(&(Common.bmp)); lastBmpLog = millis(); }
		if (Common.mpu.active && millis() - lastImuLog >= LOG_IMU_DELAY) { log_imu(&(Common.mpu)); lastImuLog = millis(); }
		if (millis() - lastMotLog >= LOG_MOT_DELAY) { log_mot(Common.mot_l, Common.mot_r); lastMotLog = millis(); }
		if (Common.gps.active && millis() - lastGpsLog >= LOG_GPS_DELAY /*Common.gps.newData*/) { log_gps(&(Common.gps)); Common.gps.newData = false; lastGpsLog = millis();}
		if (Common.radio.active && (Common.radio.newTxData || Common.radio.newRxData)) { log_radio(&(Common.radio)); }
		if (Common.sps.newData) { log_sps(&(Common.sps)); Common.sps.newData = false; }
		if (Common.operation_mode != 31 && millis() - lastTargetYawLog >= LOG_TARGET_YAW_DELAY) { log_print("TY: %.1f*", Common.target_yaw); lastTargetYawLog = millis(); }

		if (millis() - lastLogSave >= LOG_SAVE_DELAY) { log_save(); }
	#endif

	#if LOGING_PRINT_DATA
		if (millis() - lastDataPrint >= DATA_PRINT_DELAY)
		{
			#if LOGING_PRINT_SENSORS
				if (Common.bmp.active)
				{
					println("Pressure: %.4f", Common.bmp.pressure);
					println("Temperature: %.4f", Common.bmp.temperature);
					println("Altitude(P): %.1f", Common.bmp.altitude);
				}
				if (Common.gps.active && Common.gps.fix)
				{
					println("Latitude: %.6f", Common.gps.latitudeDegrees);
					println("Longitude: %.6f", Common.gps.longitudeDegrees);
					println("Altitude(G): %.1f", Common.gps.altitude);
				}
				else if (Common.gps.active) println("GPS has no fix");
				if (Common.mpu.active) println("Rotation: %.0f, %.0f, %.0f", Common.mpu.yaw, Common.mpu.pitch, Common.mpu.roll);
				if (Common.sps.active)
				{
					println("Mass concentraion: %.1f %.1f %.1f %.1f", Common.sps.pm1, Common.sps.pm2, Common.sps.pm4, Common.sps.pm10);
					println("Number concentraion: %.1f %.1f %.1f %.1f %.1f", Common.sps.n_pm05, Common.sps.n_pm1, Common.sps.n_pm2, Common.sps.n_pm4, Common.sps.n_pm10);
					println("Typical size: %.2f", Common.sps.typical_size);
				}
			#endif
			#if LOGING_PRINT_RADIO
				if (Common.radio.active)
				{
					println("Packets transmitted: %d", Common.radio.txCount);
					println("Packets received: %d", Common.radio.rxCount);
				}
			#endif
			#if LOGING_PRINT_INFO
				println("Motor state: %s", Common.motors_enabled ? "enabled" : "disabled");
				println("Motor power: %.1f %.1f", Common.mot_l, Common.mot_r);
				println("Servo state: %s", Common.servo_enabled ? "open" : "closed");
				println("Targeting to: %.4f, %.4f, %.1f at %.1f*", Common.target_lat, Common.target_lon, Common.target_alt, Common.target_yaw);
				char operation_string[10];
				switch (Common.operation_mode)
				{
					case 0:
						sprintf(operation_string, "auto"); break;
					case 1:
						sprintf(operation_string, "settarget"); break;
					case 2:
						sprintf(operation_string, "direction"); break;
					case 3:
						sprintf(operation_string, "manul"); break;
					case 31:
					default:
						sprintf(operation_string, "terminate"); break;
				}
				println("Operation mode: %s", operation_string);
			#endif
			lastDataPrint = millis();
		}
	#endif
	return;
}

#endif
