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
#include "mpu9250.h"
// ? ADD SMPLRT_DIV
static MPU9250_config mpu9250_default_config =
{
	{0.0f, 0.0f, 0.0f},	// Euler offsets
	MPU9250_AFS_2G,		// Ascale
	MPU9250_GFS_250DPS, // Gscale
	MPU9250_MFS_16BITS, // Mscale
	MPU9250_MMODE_100,	// Mmode
	
	false,				// force calibration on startup
	{12.381678, 12.381678, -923.776245},	// magbias[3]
	{0.862595, 2.022901, -1.541985},	// gyroBias[3]
	{-83.557129 / 1000.0, 5.432129 / 1000.0, 139.282227 / 1000.0},	// accelBias[3]

	1000.0,				// alg_rate [Hz]
	500.0				// euler_rate [Hz]
};

// Pressure sensor config
#include "bmp280.h"
#define SEA_PRESSURE 1002	// pressure at sea level [hPa]
static BMP280_config bmp280_default_config =
{
	BMP280_MODE_NORMAL,		// operation mode
	BMP280_FILTER_OFF,		// short-term dirsturbance filter
	BMP280_OVERSAMPLING_16,	// pressure oversampling (1:16 or none)
	BMP280_OVERSAMPLING_16,	// temperature oversampling (1:16 or none)
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
