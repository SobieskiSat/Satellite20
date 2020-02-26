#include "logger.h"

#include <stdbool.h>
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

	logBufferIndex = 0;
	bmpBufferIndex = 0;
	gpsBufferIndex = 0;
	imuBufferIndex = 0;
	motBufferIndex = 0;
	radioBufferIndex = 0;
}

void log_save()
{
	//SD_init();

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


	logBufferIndex = 0;
	bmpBufferIndex = 0;
	gpsBufferIndex = 0;
	imuBufferIndex = 0;
	motBufferIndex = 0;
	radioBufferIndex = 0;
	memset(logBuffer, 0x00, 1024);
	memset(bmpBuffer, 0x00, 1024);
	memset(gpsBuffer, 0x00, 1024);
	memset(imuBuffer, 0x00, 1024);
	memset(motBuffer, 0x00, 1024);
	memset(radioBuffer, 0x00, 1024);
}


void log_print(char* line)
{
	if (logBufferIndex < 1024)
	{
		strcat(logBuffer, line);
		logBufferIndex = strlen(logBuffer);
	}
}

void log_bmp(BMP280* bmp)
{
	if (bmpBufferIndex < 1024)
	{
		sprintf(timestamp, "\t@%lu\r\n", millis());
		sprintf(tempBuffer, "%.02f %.02f", bmp->pressure, bmp->temperature);
		strcat(tempBuffer, timestamp);
		strcat(bmpBuffer, tempBuffer);
		bmpBufferIndex = strlen(bmpBuffer);

		memset(tempBuffer, 0x00, 1024);
	}
}
void log_gps(GPS* gps)
{
	if (gpsBufferIndex < 1024)
	{
		sprintf(timestamp, "\t@%lu\r\n", millis());
		sprintf(tempBuffer, "%.07f %.07f", gps->latitudeDegrees, gps->longitudeDegrees);
		strcat(tempBuffer, timestamp);
		strcat(gpsBuffer, tempBuffer);
		gpsBufferIndex = strlen(gpsBuffer);

		memset(tempBuffer, 0x00, 1024);
	}
}
void log_imu(float* eulers)
{
	if (imuBufferIndex < 1024)
	{
		sprintf(timestamp, "\t@%lu\r\n", millis());
		sprintf(tempBuffer, "%.02f %.02f %.02f", eulers[0], eulers[1], eulers[2]);
		strcat(tempBuffer, timestamp);
		strcat(imuBuffer, tempBuffer);
		imuBufferIndex = strlen(imuBuffer);

		memset(tempBuffer, 0x00, 1024);
	}
}

void log_mot(float left, float right)
{
	if (motBufferIndex < 1024)
	{
		sprintf(timestamp, "\t@%lu\r\n", millis());
		sprintf(tempBuffer, "%.01f %.01f", left, right);
		strcat(tempBuffer, timestamp);
		strcat(motBuffer, tempBuffer);
		motBufferIndex = strlen(motBuffer);

		memset(tempBuffer, 0x00, 1024);
	}
}
void log_radio(SX1278* radio, bool transmit)
{
	if (radioBufferIndex < 1024)
	{
		sprintf(timestamp, "]\t@%lu\r\n", millis());

		if (transmit)
		{
			sprintf(tempBuffer, "TX\t[");
		}
		else
		{
			sprintf(tempBuffer, "RX\t%d\t[", radio->rssi);
		}
		strcat(tempBuffer, radio->lastPacket);

		strcat(tempBuffer, timestamp);
		strcat(radioBuffer, tempBuffer);
		radioBufferIndex = strlen(radioBuffer);

		memset(tempBuffer, 0x00, 1024);
	}
}
