#include "bmp280.h"

static inline int read_data(BMP280 *inst, uint8_t addr, uint8_t *value, uint8_t len) {
	uint16_t tx_buff;
	tx_buff = (inst->addr << 1);

	if (HAL_I2C_Mem_Read(inst->i2c, tx_buff, addr, 1, value, len, 5000) == HAL_OK) return 0;
	else return 1;
}

static int write_register8(BMP280 *inst, uint8_t addr, uint8_t value) {
	uint16_t tx_buff;

	tx_buff = (inst->addr << 1);

	if (HAL_I2C_Mem_Write(inst->i2c, tx_buff, addr, 1, &value, 1, 10000) == HAL_OK) return false;
	else return true;
}

static bool read_register16(BMP280 *inst, uint8_t addr, uint16_t *value) {
	uint16_t tx_buff;
	uint8_t rx_buff[2];
	tx_buff = (inst->addr << 1);

	if (HAL_I2C_Mem_Read(inst->i2c, tx_buff, addr, 1, rx_buff, 2, 5000) == HAL_OK)
	{
		*value = (uint16_t) ((rx_buff[1] << 8) | rx_buff[0]);
		return true;
	}
	else return false;

}

static bool read_calibration_data(BMP280 *inst)
{
	return (read_register16(inst, 0x88, &inst->dig_T1)
	&& read_register16(inst, 0x8a, (uint16_t *) &inst->dig_T2)
	&& read_register16(inst, 0x8c, (uint16_t *) &inst->dig_T3)
	&& read_register16(inst, 0x8e, &inst->dig_P1)
	&& read_register16(inst, 0x90, (uint16_t *) &inst->dig_P2)
	&& read_register16(inst, 0x92, (uint16_t *) &inst->dig_P3)
	&& read_register16(inst, 0x94, (uint16_t *) &inst->dig_P4)
	&& read_register16(inst, 0x96, (uint16_t *) &inst->dig_P5)
	&& read_register16(inst, 0x98, (uint16_t *) &inst->dig_P6)
	&& read_register16(inst, 0x9a, (uint16_t *) &inst->dig_P7)
	&& read_register16(inst, 0x9c, (uint16_t *) &inst->dig_P8)
	&& read_register16(inst, 0x9e, (uint16_t *) &inst->dig_P9));
}

bool bmp280_init(BMP280 *inst, BMP280_config *params)
{
	if (inst->addr != BMP280_I2C_ADDRESS_0 && inst->addr != BMP280_I2C_ADDRESS_1) return false;
	if (read_data(inst, BMP280_REG_ID, &inst->id, 1)) return false;
	if (inst->id != BMP280_CHIP_ID) return false;

	// Soft reset.
	if (write_register8(inst, BMP280_REG_RESET, BMP280_RESET_VALUE)) return false;

	// Wait until finished copying over the NVP data.
	while (1) {
		uint8_t status;
		if (!read_data(inst, BMP280_REG_STATUS, &status, 1) && (status & 1) == 0) break;
	}

	if (!read_calibration_data(inst)) return false;

	uint8_t config = (params->standby << 5) | (params->filter << 2);
	if (write_register8(inst, BMP280_REG_CONFIG, config)) return false;

	if (params->mode == BMP280_MODE_FORCED) {
		params->mode = BMP280_MODE_SLEEP;  // initial mode for forced is sleep
	}

	uint8_t ctrl = (params->oversampling_temperature << 5) | (params->oversampling_pressure << 2) | (params->mode);

	if (write_register8(inst, BMP280_REG_CTRL, ctrl)) return false;

	return true;
}

bool bmp280_is_measuring(BMP280 *inst) {
	uint8_t status;
	if (read_data(inst, BMP280_REG_STATUS, &status, 1)) return false;

	if (status & (1 << 3)) return true;

	return false;
}

/**
 * Compensation algorithm is taken from BMP280 datasheet.
 *
 * Return value is in degrees Celsius.
 */
static inline int32_t compensate_temperature(BMP280 *inst, int32_t adc_temp,
		int32_t *fine_temp) {
	int32_t var1, var2;

	var1 = ((((adc_temp >> 3) - ((int32_t) inst->dig_T1 << 1)))
			* (int32_t) inst->dig_T2) >> 11;
	var2 = (((((adc_temp >> 4) - (int32_t) inst->dig_T1)
			* ((adc_temp >> 4) - (int32_t) inst->dig_T1)) >> 12)
			* (int32_t) inst->dig_T3) >> 14;

	*fine_temp = var1 + var2;
	return (*fine_temp * 5 + 128) >> 8;
}

/**
 * Compensation algorithm is taken from BMP280 datasheet.
 *
 * Return value is in Pa, 24 integer bits and 8 fractional bits.
 */
static inline uint32_t compensate_pressure(BMP280 *inst, int32_t adc_press,
		int32_t fine_temp) {
	int64_t var1, var2, p;

	var1 = (int64_t) fine_temp - 128000;
	var2 = var1 * var1 * (int64_t) inst->dig_P6;
	var2 = var2 + ((var1 * (int64_t) inst->dig_P5) << 17);
	var2 = var2 + (((int64_t) inst->dig_P4) << 35);
	var1 = ((var1 * var1 * (int64_t) inst->dig_P3) >> 8)
			+ ((var1 * (int64_t) inst->dig_P2) << 12);
	var1 = (((int64_t) 1 << 47) + var1) * ((int64_t) inst->dig_P1) >> 33;

	if (var1 == 0) {
		return 0;  // avoid exception caused by division by zero
	}

	p = 1048576 - adc_press;
	p = (((p << 31) - var2) * 3125) / var1;
	var1 = ((int64_t) inst->dig_P9 * (p >> 13) * (p >> 13)) >> 25;
	var2 = ((int64_t) inst->dig_P8 * p) >> 19;

	p = ((p + var1 + var2) >> 8) + ((int64_t) inst->dig_P7 << 4);
	return p;
}

bool bmp280_read_fixed(BMP280 *inst, int32_t *temperature, uint32_t *pressure) {
	int32_t adc_pressure;
	int32_t adc_temp;
	uint8_t data[6];

	// Need to read in one sequence to ensure they match.
	size_t size = 6;
	if (read_data(inst, 0xf7, data, size)) return false;

	adc_pressure = data[0] << 12 | data[1] << 4 | data[2] >> 4;
	adc_temp = data[3] << 12 | data[4] << 4 | data[5] >> 4;

	int32_t fine_temp;
	*temperature = compensate_temperature(inst, adc_temp, &fine_temp);
	*pressure = compensate_pressure(inst, adc_pressure, fine_temp);

	return true;
}

bool bmp280_read_float(BMP280 *inst, float *temperature, float *pressure)
{
	int32_t fixed_temperature;
	uint32_t fixed_pressure;
	if (bmp280_read_fixed(inst, &fixed_temperature, &fixed_pressure))
	{
		*temperature = (float) fixed_temperature / 100;
		*pressure = (float) fixed_pressure / 256;

		return true;
	}

	return false;
}
