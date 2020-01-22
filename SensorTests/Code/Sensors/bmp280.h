#ifndef SENSORS_BMP280_H_
#define SENSORS_BMP280_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

/**
 * Mode of BMP280 module operation.
 * Forced - Measurement is initiated by user.
 * Normal - Continues measurement.
 */
typedef enum {
    BMP280_MODE_SLEEP = 0,
    BMP280_MODE_FORCED = 1,
    BMP280_MODE_NORMAL = 3
} BMP280_Mode;

typedef enum {
    BMP280_FILTER_OFF = 0,
    BMP280_FILTER_2 = 1,
    BMP280_FILTER_4 = 2,
    BMP280_FILTER_8 = 3,
    BMP280_FILTER_16 = 4
} BMP280_Filter;

/**
 * Pressure oversampling settings
 */
typedef enum {
    BMP280_SKIPPED = 0,          /* no measurement  */
    BMP280_ULTRA_LOW_POWER = 1,  /* oversampling x1 */
    BMP280_LOW_POWER = 2,        /* oversampling x2 */
    BMP280_STANDARD = 3,         /* oversampling x4 */
    BMP280_HIGH_RES = 4,         /* oversampling x8 */
    BMP280_ULTRA_HIGH_RES = 5    /* oversampling x16 */
} BMP280_Oversampling;

/**
 * Stand by time between measurements in normal mode
 */
typedef enum {
    BMP280_STANDBY_05 = 0,      /* stand by time 0.5ms */
    BMP280_STANDBY_62 = 1,      /* stand by time 62.5ms */
    BMP280_STANDBY_125 = 2,     /* stand by time 125ms */
    BMP280_STANDBY_250 = 3,     /* stand by time 250ms */
    BMP280_STANDBY_500 = 4,     /* stand by time 500ms */
    BMP280_STANDBY_1000 = 5,    /* stand by time 1s */
    BMP280_STANDBY_2000 = 6,    /* stand by time 2s BMP280, 10ms BME280 */
    BMP280_STANDBY_4000 = 7,    /* stand by time 4s BMP280, 20ms BME280 */
} BMP280_StandbyTime;

/**
 * Configuration parameters for BMP280 module.
 * Use function bmp280_init_default_params to use default configuration.
 */
typedef struct {
    BMP280_Mode mode;
    BMP280_Filter filter;
    BMP280_Oversampling oversampling_pressure;
    BMP280_Oversampling oversampling_temperature;
    BMP280_StandbyTime standby;
} BMP280_config;

typedef struct {

	I2C_HandleTypeDef* i2c;
    uint16_t addr;
    uint8_t  id;        /* Chip ID */

    BMP280_config params;
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;

} BMP280;

// default config
static inline void bmp280_get_default_config(BMP280_config *inst_params) {
	inst_params->mode = BMP280_MODE_NORMAL;
	inst_params->filter = BMP280_FILTER_OFF;
	inst_params->oversampling_pressure = BMP280_ULTRA_HIGH_RES;
	inst_params->oversampling_temperature = BMP280_ULTRA_HIGH_RES;
	inst_params->standby = BMP280_STANDBY_05;
};

bool bmp280_init(BMP280 *inst, BMP280_config *params);
bool bmp280_is_measuring(BMP280 *inst);
bool bmp280_read_fixed(BMP280 *inst, int32_t *temperature, uint32_t *pressure);
bool bmp280_read_float(BMP280 *inst, float *temperature, float *pressure);


#define BMP280_I2C_ADDRESS_0  0x76
#define BMP280_I2C_ADDRESS_1  0x77

#define BMP280_CHIP_ID  0x58 /* BMP280 has chip-id 0x58 */

#define BMP280_REG_TEMP_XLSB   0xFC /* bits: 7-4 */
#define BMP280_REG_TEMP_LSB    0xFB
#define BMP280_REG_TEMP_MSB    0xFA
#define BMP280_REG_TEMP        (BMP280_REG_TEMP_MSB)
#define BMP280_REG_PRESS_XLSB  0xF9 /* bits: 7-4 */
#define BMP280_REG_PRESS_LSB   0xF8
#define BMP280_REG_PRESS_MSB   0xF7
#define BMP280_REG_PRESSURE    (BMP280_REG_PRESS_MSB)
#define BMP280_REG_CONFIG      0xF5 /* bits: 7-5 t_sb; 4-2 filter; 0 spi3w_en */
#define BMP280_REG_CTRL        0xF4 /* bits: 7-5 osrs_t; 4-2 osrs_p; 1-0 mode */
#define BMP280_REG_STATUS      0xF3 /* bits: 3 measuring; 0 im_update */
#define BMP280_REG_CTRL_HUM    0xF2 /* bits: 2-0 osrs_h; */
#define BMP280_REG_RESET       0xE0
#define BMP280_REG_ID          0xD0
#define BMP280_REG_CALIB       0x88
#define BMP280_REG_HUM_CALIB   0x88

#define BMP280_RESET_VALUE     0xB6

#endif /* SENSORS_BMP280_H_ */