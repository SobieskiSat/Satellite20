#ifndef CONFIG_H_
#define CONFIG_H_

#include "stm32f4xx_hal.h"

#define PWM_RESOLUTION 256
#define MOTOR_L_DIR 1
#define MOTOR_R_DIR 1

#include "bmp280.h"

static BMP280_config bmp280_default_config = {
	BMP280_MODE_NORMAL,
	BMP280_FILTER_OFF,
	BMP280_ULTRA_HIGH_RES, //pressure
	BMP280_ULTRA_HIGH_RES, //temperature
	BMP280_STANDBY_05};

#include "sx1278.h"

static SX1278_config sx1278_default_config = {
	434000000,	//frequency in Hz, radio resolution: 61.035Hz (in code resolution may be different)
	SX1278_POWER_17DBM,
	SX1278_SF_7,
	SX1278_CR_4_5,
	SX1278_BW_125KHZ,
	SX1278_CRC_DIS,
	100	// rxTimeout = val * 1.024ms (for SF=7, BW=125K) [rxTimeout = val * (2^(SF) / BW)]
	};

#endif /* CONFIG_H_ */
