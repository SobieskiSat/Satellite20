#ifndef CONFIG_H_
#define CONFIG_H_

#include "stm32f4xx_hal.h"

// Radio config
#include "sx1278.h"
static SX1278_config sx1278_default_config =
{
	434000000,			// radio frequency (d=61.035)[Hz]
	SX1278_POWER_17DBM,	// output power (11:20)[dBm]
	SX1278_SF_7,		// spreading factor (6:12)
	SX1278_CR_4_5,		// coding rate (5:8)
	SX1278_BW_125KHZ,	// bandwidth (8:500)[kHz]
	SX1278_CRC_EN,		// verify packet checksum at reception
	50					// receive timeout [ms]
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
	BMP280_STANDBY_125		// standby between measurements (05:4000)[ms]
};

// IMU config
#include "mpu9250.h"
// ? ADD SMPLRT_DIV
static MPU9250_config mpu9250_default_config =
{
	{90.0f, 0.0f, 0.0f},	// Euler offsets
	MPU9250_AFS_2G,		// Ascale
	MPU9250_GFS_250DPS, // Gscale
	MPU9250_MFS_16BITS, // Mscale
	MPU9250_MMODE_100,	// Mmode
	
	true,				// force calibration on startup
	{12.381678, 12.381678, -923.776245},	// magbias[3]
	{0.862595, 2.022901, -1.541985},	// gyroBias[3]
	{-83.557129 / 1000.0, 5.432129 / 1000.0, 139.282227 / 1000.0},	// accelBias[3]

	-1,					// alg_rate [Hz]
	100.0				// euler_rate [Hz]
};

// Duplex
#define DUPLEX_TX_COUNT 5			// Count of packets transmitted per one received

// Timing parameters [ms]
#define DATA_PRINT_DELAY 1000
#define LOG_SAVE_DELAY 2373
#define LOG_BMP_DELAY 100
#define LOG_IMU_DELAY 100
#define LOG_MOT_DELAY 100
#define LOG_GPS_DELAY 2000
#define LOG_TARGET_YAW_DELAY 1000
// Radio timeout value is set in SX1278 config 
#define SENSING_BMP_DELAY 50
#define SENSING_SPS_DELAY 1200
#define STEERING_PID_DELAY 10		// Delay between motor thrust updates
#define STEERING_YAW_DELAY 100		// Delay between target yaw corrections

// Peripherial state
#define SD_ENABLE 0
#define RADIO_ENABLE 0
#define BMP_ENABLE 0
#define GPS_ENABLE 0
#define IMU_ENABLE 0
#define SPS_ENABLE 1
#define STEERING_ENABLE 0	// defines MOTOR_ENABLE

// Debug messages
#define RUN_DEBUG 0
#define LOGING_DEBUG 0
#define LOGING_PRINT_DATA 1
	#define LOGING_PRINT_SENSORS 1
	#define LOGING_PRINT_RADIO 0
	#define LOGING_PRINT_INFO 0
#define DUPLEX_DEBUG 0
#define SENSING_DEBUG 1
#define STEERING_DEBUG 0

// Flight parameters
#define FLIGHT_START_THRE 16
#define DEFAULT_TARGET_LAT 50.053530
#define DEFAULT_TARGET_LON 19.935201
#define DEFAULT_TARGET_ALT 261.0
#define DEFAULT_TARGET_YAW 0.0
#define KEEPOUT_LAT 0.0001	// Destination range (area)
#define KEEPOUT_LON 0.0001
#define KEEPOUT_ALT 25.0
#define TERMINAL_HOR 45.0	// Maximum horizonal angle to be accepted
#define TERMINAL_YAW_DX 40.0	// Maximum yaw angular speed in (deg/s) to be accepted
#define TERMINAL_ALT_DX -15.0	// Maximum falling speed to be accepted (m/s)
// PID
#define PID_kp 1.0 		//dobrany
#define PID_ki 0.5
#define PID_kd 500.0 	//dobrany 500 ok - 100ms

// Motor config
#define MOTOR_L_DIR 0
#define MOTOR_R_DIR 0
#define MOTOR_PWM_RESOLUTION 1024

#endif /* CONFIG_H_ */
