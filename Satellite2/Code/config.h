#ifndef CONFIG_H_
#define CONFIG_H_

#include "stm32f4xx_hal.h"

#include "bmp280.h"

static BMP280_config bmp280_default_config = {
	BMP280_MODE_NORMAL,
	BMP280_FILTER_OFF,
	BMP280_ULTRA_HIGH_RES, //pressure
	BMP280_ULTRA_HIGH_RES, //temperature
	BMP280_STANDBY_05};

#define RADIO_SX1278
//#define RADIO_SX1280

#ifdef RADIO_SX1278
#include "sx1278.h"

static SX1278_config sx1278_default_config = {
	433.0,	//note: frequency must be in MHz
	SX1278_POWER_17DBM,
	SX1278_SF_8,
	SX1278_CR_4_5,
	SX1278_BW_20_8KHZ,
	SX1278_CRC_EN };
#endif

#ifdef RADIO_SX1280

#endif

#endif /* CONFIG_H_ */
