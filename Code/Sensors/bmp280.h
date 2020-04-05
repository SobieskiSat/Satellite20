#ifndef SENSORS_BMP280_H_
#define SENSORS_BMP280_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

// Configuration parameters
typedef struct
{
	uint8_t mode;
	uint8_t filter;
	uint8_t oversampling_pressure;
	uint8_t oversampling_temperature;
	uint8_t standby;
} BMP280_config;

typedef struct //BMP280
{
	I2C_HandleTypeDef *i2c;
	uint16_t i2c_addr;

	BMP280_config params;
	float pressure, temperature, altitude;
	bool active;

	// reading compensation variables
	uint16_t dig_T1;
	int16_t dig_T2, dig_T3;
	uint16_t dig_P1;
	int16_t dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
} BMP280;

bool bmp280_init(BMP280* inst, BMP280_config* params);
bool bmp280_update(BMP280* inst);
bool bmp280_present(BMP280* inst, uint8_t trials);
void bmp280_reset(BMP280* inst);
bool bmp280_is_measuring(BMP280* inst);

// Mode of operation
#define BMP280_MODE_SLEEP 0
#define BMP280_MODE_FORCED 1 // measurement is initiated by user
#define BMP280_MODE_NORMAL 3 // continues measurement
// Short-term disturbance filter options
#define BMP280_FILTER_OFF 0
#define BMP280_FILTER_2 1
#define BMP280_FILTER_4 2
#define BMP280_FILTER_8 3
#define BMP280_FILTER_16 4
// Oversampling options
#define BMP280_OVERSAMPLING_NONE 0
#define BMP280_OVERSAMPLING_1 1
#define BMP280_OVERSAMPLING_2 2
#define BMP280_OVERSAMPLING_4 3
#define BMP280_OVERSAMPLING_8 4
#define BMP280_OVERSAMPLING_16 5
// Standby time between measurements in normal mode [ms]
#define BMP280_STANDBY_05 0
#define BMP280_STANDBY_62 1
#define BMP280_STANDBY_125 2
#define BMP280_STANDBY_250 3
#define BMP280_STANDBY_500 4
#define BMP280_STANDBY_1000 5
#define BMP280_STANDBY_2000 6
#define BMP280_STANDBY_4000 7
// Chip parameters
#define BMP280_SDO_STATE 0 // pull state on SDO pin
#define BMP280_I2C_ADDRESS ((0x76 & BMP280_SDO_STATE) << 1)
#define BMP280_CHIP_ID 0x58
#define BMP280_RESET_VALUE 0xB6
// Register adresses
#define BMP280_REG_TEMP_XLSB 0xFC // bits: 7-4
#define BMP280_REG_TEMP_LSB 0xFB
#define BMP280_REG_TEMP_MSB 0xFA
#define BMP280_REG_TEMP (BMP280_REG_TEMP_MSB)
#define BMP280_REG_PRESS_XLSB 0xF9 // bits: 7-4
#define BMP280_REG_PRESS_LSB 0xF8
#define BMP280_REG_PRESS_MSB 0xF7
#define BMP280_REG_PRESSURE (BMP280_REG_PRESS_MSB)
#define BMP280_REG_CONFIG 0xF5	 // bits: 7-5 t_sb; 4-2 filter; 0 spi3w_en
#define BMP280_REG_CTRL 0xF4	 // bits: 7-5 osrs_t; 4-2 osrs_p; 1-0 mode
#define BMP280_REG_STATUS 0xF3	 // bits: 3 measuring; 0 im_update
#define BMP280_REG_CTRL_HUM 0xF2 // bits: 2-0 osrs_h;
#define BMP280_REG_RESET 0xE0
#define BMP280_REG_ID 0xD0
#define BMP280_REG_CALIB 0x88
#define BMP280_REG_HUM_CALIB 0x88

#endif /* SENSORS_BMP280_H_ */
