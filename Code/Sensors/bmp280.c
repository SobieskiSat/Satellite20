#include "bmp280.h"
#include "run.h"
#include "config.h"

static bool BMP280_readBytes(BMP280* inst, uint8_t mem_addr, uint8_t* data, uint8_t len)
{
	return (HAL_I2C_Mem_Read(inst->i2c, inst->i2c_addr, mem_addr, 1, data, len, 5) == HAL_OK);
}
static bool BMP280_writeByte(BMP280* inst, uint8_t mem_addr, uint8_t data)
{
	return (HAL_I2C_Mem_Write(inst->i2c, inst->i2c_addr, mem_addr, 1, &data, 1, 5) == HAL_OK);
}
static bool BMP280_read16(BMP280* inst, uint8_t mem_addr, uint16_t* data)
{
	uint8_t rx_buff[2];

	if (HAL_I2C_Mem_Read(inst->i2c, inst->i2c_addr, mem_addr, 1, rx_buff, 2, 5) == HAL_OK)
	{
		*data = (uint16_t)((rx_buff[1] << 8) | rx_buff[0]);
		return true;
	}
	else return false;
}

static bool read_calibration_data(BMP280* inst)
{
	return(BMP280_read16(inst, 0x88, &inst->dig_T1)
		&& BMP280_read16(inst, 0x8a, (uint16_t*) &inst->dig_T2)
		&& BMP280_read16(inst, 0x8c, (uint16_t*) &inst->dig_T3)
		&& BMP280_read16(inst, 0x8e, &inst->dig_P1)
		&& BMP280_read16(inst, 0x90, (uint16_t*) &inst->dig_P2)
		&& BMP280_read16(inst, 0x92, (uint16_t*) &inst->dig_P3)
		&& BMP280_read16(inst, 0x94, (uint16_t*) &inst->dig_P4)
		&& BMP280_read16(inst, 0x96, (uint16_t*) &inst->dig_P5)
		&& BMP280_read16(inst, 0x98, (uint16_t*) &inst->dig_P6)
		&& BMP280_read16(inst, 0x9a, (uint16_t*) &inst->dig_P7)
		&& BMP280_read16(inst, 0x9c, (uint16_t*) &inst->dig_P8)
		&& BMP280_read16(inst, 0x9e, (uint16_t*) &inst->dig_P9));
}

static int32_t compensate_temperature(BMP280 *inst, int32_t adc_temp, int32_t *fine_temp)
{
	int32_t var1, var2;

	var1 = ((((adc_temp >> 3) - ((int32_t) inst->dig_T1 << 1)))
			* (int32_t) inst->dig_T2) >> 11;
	var2 = (((((adc_temp >> 4) - (int32_t) inst->dig_T1)
			* ((adc_temp >> 4) - (int32_t) inst->dig_T1)) >> 12)
			* (int32_t) inst->dig_T3) >> 14;

	*fine_temp = var1 + var2;
	return (*fine_temp * 5 + 128) >> 8;
}

static uint32_t compensate_pressure(BMP280 *inst, int32_t adc_press, int32_t fine_temp)
{
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

void bmp280_reset(BMP280* inst)
{
	// soft reset
	BMP280_writeByte(inst, BMP280_REG_RESET, BMP280_RESET_VALUE);
	delay(100);
}

bool bmp280_present(BMP280* inst, uint8_t trials)
{
	// request and verify chip id
	uint8_t attempts = 0;
	do
	{
		bmp280_reset(inst);
		uint8_t who[1] = { 0x00 };
		BMP280_readBytes(inst, BMP280_REG_ID, who, 1);
		if (who[0] == BMP280_CHIP_ID) return true;
		else
		{
			attempts++;
			delay(1000);
		}
	} while (attempts <= trials);
	inst->active = false;
	return false;
}

bool bmp280_init(BMP280* inst, BMP280_config* params)
{
	uint8_t temp_data[1];	// nasty temporary byte, used for everything
	inst->active = false;
	inst->params = *params;

	if (!bmp280_present(inst, 1)) return false;
	bmp280_reset(inst);

	while (!(BMP280_readBytes(inst, BMP280_REG_STATUS, temp_data, 1) && (temp_data[0] & 0x01) == 0)); // wait until finished copying over the NVM data

	if (!read_calibration_data(inst)) return false;

	// Setting parameters
	temp_data[0] = (params->standby << 5) | (params->filter << 2);	// config byte
	if (!BMP280_writeByte(inst, BMP280_REG_CONFIG, temp_data[0])) return false;

	if (params->mode == BMP280_MODE_FORCED) params->mode = BMP280_MODE_SLEEP;  // initial mode for forced is sleep
	temp_data[0] = (params->oversampling_temperature << 5) | (params->oversampling_pressure << 2) | (params->mode); // ctrl byte
	if (!BMP280_writeByte(inst, BMP280_REG_CTRL, temp_data[0])) return false;

	inst->active = true;
	return true;
}

bool bmp280_update(BMP280 *inst)
{
	int32_t adc_pressure;
	int32_t adc_temp;
	uint8_t data[6];

	size_t size = 6;
	if (!BMP280_readBytes(inst, 0xf7, data, size)) return false;

	adc_pressure = data[0] << 12 | data[1] << 4 | data[2] >> 4;
	adc_temp = data[3] << 12 | data[4] << 4 | data[5] >> 4;

	int32_t fine_temp;
	inst->temperature = (float)(compensate_temperature(inst, adc_temp, &fine_temp)) / 100.0;
	inst->pressure = (float)(compensate_pressure(inst, adc_pressure, fine_temp)) / 25600.0;
	inst->altitude = 44330 * (1.0 - pow(inst->pressure / SEA_PRESSURE, 0.1903));

	return true;
}

bool bmp280_is_measuring(BMP280 *inst)
{
	uint8_t status;
	return (BMP280_readBytes(inst, BMP280_REG_STATUS, &status, 1) && ((status >> 3) == 1));
}
