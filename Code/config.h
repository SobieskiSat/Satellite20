#ifndef CONFIG_H_
#define CONFIG_H_

#include "stm32f4xx_hal.h"

// Motor config
#define MOTOR_L_DIR 0
#define MOTOR_R_DIR 0
#define MOTOR_PWM_RESOLUTION 1024

// Radio config
#include "sx1278.h"
static SX1278_config sx1278_default_config =
{
	434000000,			// radio frequency (d=61.035)[Hz]
	SX1278_POWER_17DBM,	// output power (11:20)[dBm]
	SX1278_SF_7,		// spreading factor (6:12)
	SX1278_CR_4_5,		// coding rate (5:8)
	SX1278_BW_125KHZ,	// bandwidth (8:500)[kHz]
	SX1278_CRC_DIS,		// ??
	50					// receive timeout [ms]
};

// IMU config
#define IMU_CALIBRATE 1	// force calibration on startup
#define YAW_OFFSET 0.0	// offset from real magnetic north [deg]

// Pressure sensor config
#include "bmp280.h"
static BMP280_config bmp280_default_config =
{
	BMP280_MODE_NORMAL,		// operation mode
	BMP280_FILTER_OFF,		// short-term dirsturbance filter
	BMP280_ULTRA_HIGH_RES,	// pressure resolution
	BMP280_ULTRA_HIGH_RES,	// temperature resolution
	BMP280_STANDBY_05		// standby between measurements (05:4000)[ms]
};


// Peripherial state (dangerous!, maybe to implement)
#define MOTOR_ENABLE 1
#define RADIO_ENABLE 1
#define IMU_ENABLE 1
#define GPS_ENABLE 1
#define BMP_ENABLE 1
#define SD_ENABLE 1

#endif /* CONFIG_H_ */
