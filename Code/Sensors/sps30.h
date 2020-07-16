#ifndef SENSORS_SPS30_H_
#define SENSORS_SPS30_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "clock.h"

typedef struct
{
	UART_HandleTypeDef* uart;

	float pm1, pm2, pm4, pm10;					// Mass concentration
	float n_pm05, n_pm1, n_pm2, n_pm4, n_pm10;	// Number concentration
	float typical_size; 						// Typical particle size

	uint8_t txBuffer[16];
	uint8_t rxBuffer[255];
	uint8_t rxData[255];
	uint8_t rxLen;
	uint8_t checksum;

	bool newData, active;

} SPS30;

bool SPS30_init(SPS30* inst);
bool SPS30_update(SPS30* inst);

void SPS30_start_meas(SPS30* inst);
void SPS30_stop_meas(SPS30* inst);

void SPS30_reset(SPS30* inst);
bool SPS30_present(SPS30* inst);
void SPS30_clean(SPS30* inst);

#define SPS30_START_MEAS	0x00
#define SPS30_STOP_MEAS		0x01
#define SPS30_READ_MEAS		0x03
#define SPS30_RW_AUTO_CLEAN	0x80
#define SPS30_START_FAN		0x56
#define SPS30_DEVICE_INFO	0xD0
#define SPS30_RESET			0xD3

#endif /* SENSORS_SPS30_H_ */
