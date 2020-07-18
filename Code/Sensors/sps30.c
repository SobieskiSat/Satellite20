#include "sps30.h"

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "run.h"
#include "clock.h"

#define SPS30_DEBUG 1

static bool SPS30_read(SPS30* inst)
{
	uint32_t temp_checksum = 0;
	uint8_t incoming[1] = {0};
	HAL_UART_Receive(inst->uart, incoming, 1, 5); if (incoming[0] != 0x7E) return false; // Start byte
	HAL_UART_Receive(inst->uart, incoming, 1, 5); if (incoming[0] != 0x00) return false;	// Address byte
	HAL_UART_Receive(inst->uart, incoming, 1, 5); temp_checksum += incoming[0];			// Command byte
	HAL_UART_Receive(inst->uart, incoming, 1, 5);
	if (incoming[0] != 0x00)						// State byte (contains errors)
	{
		#if SPS30_DEBUG
			println("[SPS] Read error: code 0x%x", (uint8_t)incoming[0]);
		#endif
		return false;
	}
	temp_checksum += incoming[0];
	HAL_UART_Receive(inst->uart, incoming, 1, 5);	inst->rxLen = incoming[0]; // Lenght byte
	temp_checksum += incoming[0];

	if (inst->rxLen > 0)
	{
		memset(inst->rxBuffer, 0x00, 255);				// Clear buffer and receive stuffed data
		if (HAL_UART_Receive(inst->uart, inst->rxBuffer, inst->rxLen, 5) != HAL_OK) return false;
	}
	HAL_UART_Receive(inst->uart, incoming, 1, 5); inst->checksum = incoming[0];	// Checksum byte
	HAL_UART_Receive(inst->uart, incoming, 1, 5); if (incoming[0] != 0x7E) return false;	// Stop byte

	uint8_t BD = 0;	// byte index of rxData array
	memset(inst->rxData, 0x00, 255);				// Data un-stuffing, clear buffer
	for (uint8_t B = 0; B < inst->rxLen; B++)
	{
		if (inst->rxBuffer[B] == 0x7D)
		{
			B++;
			switch (inst->rxBuffer[B])				// Values for un-stuffing are from SPS30 datasheet
			{
				case 0x5E:
					inst->rxData[BD] = 0x7E;
					break;
				case 0x5D:
					inst->rxData[BD] = 0x7D;
					break;
				case 0x31:
					inst->rxData[BD] = 0x11;
					break;
				case 0x33:
					inst->rxData[BD] = 0x13;
					break;
				default:
					break;
			}
			//inst->rxData[BD] = inst->rxBuffer[B] + 0x20;	// [!!!] Shortcut, may not work
		}
		else
		{
			inst->rxData[BD] = inst->rxBuffer[B];
		}
		temp_checksum += inst->rxData[BD];
		BD++;
	}
	if (inst->checksum != (uint8_t)~((temp_checksum & 0xFF))) // Check checksum
	{
		#if SPS30_DEBUG
			println("[SPS] Read error: bad checksum");
		#endif
		return false;
	}
	else return true;
}
static bool SPS30_write(SPS30* inst, uint8_t* buffer, uint8_t len)
{
	uint8_t temp[2] = {0};
	inst->checksum = 0;
	for (int B = 0; B < len; B++)
	{
		inst->checksum += buffer[B];
	}
	inst->checksum = ~(inst->checksum);

	temp[0] = 0x7E; temp[1] = 0x00;
	HAL_UART_Transmit(inst->uart, temp, 2, 5);

	HAL_UART_Transmit(inst->uart, buffer, len, 5);				// transmit bytes
	while (HAL_UART_GetState(inst->uart) != HAL_UART_STATE_READY);	// wait for finished transmission

	temp[0] = inst->checksum; temp[1] = 0x7E;
	HAL_UART_Transmit(inst->uart, temp, 2, 5);
	return true;
}

bool SPS30_init(SPS30* inst)
{

	if (!SPS30_present(inst)) return false;

	delay(400);
	SPS30_reset(inst);
	delay(400);

	SPS30_start_meas(inst);

	inst->active = true;

	//SPS30_clean(inst);

	return inst->active;
}

bool SPS30_update(SPS30* inst)
{
	//SPS30_stop_meas(inst);
	// Read measurement
	inst->txBuffer[0] = SPS30_READ_MEAS;
	inst->txBuffer[1] = 0x00;
	SPS30_write(inst, inst->txBuffer, 2);
	if (SPS30_read(inst) && inst->rxLen >= 0x28)
	{
		bytesToFloatRev(inst->rxData + 0, &(inst->pm1));
		bytesToFloatRev(inst->rxData + 4, &(inst->pm2));
		bytesToFloatRev(inst->rxData + 8, &(inst->pm4));
		bytesToFloatRev(inst->rxData + 12, &(inst->pm10));
		bytesToFloatRev(inst->rxData + 16, &(inst->n_pm05));
		bytesToFloatRev(inst->rxData + 20, &(inst->n_pm1));
		bytesToFloatRev(inst->rxData + 24, &(inst->n_pm2));
		bytesToFloatRev(inst->rxData + 28, &(inst->n_pm4));
		bytesToFloatRev(inst->rxData + 32, &(inst->n_pm10));
		bytesToFloatRev(inst->rxData + 36, &(inst->typical_size));
		inst->newData = true;
	}
	#if SPS30_DEBUG
		if (inst->rxLen == 0x00) println("[SPS] No new data to read");
	#endif
	//SPS30_start_meas(inst);
	return inst->newData;
}

void SPS30_start_meas(SPS30* inst)
{
	inst->txBuffer[0] = SPS30_START_MEAS;
	inst->txBuffer[1] = 0x02;
	inst->txBuffer[2] = 0x01;
	inst->txBuffer[3] = 0x03;
	SPS30_write(inst, inst->txBuffer, 4);
	SPS30_read(inst);
}

void SPS30_stop_meas(SPS30* inst)
{
	inst->txBuffer[0] = SPS30_STOP_MEAS;
	inst->txBuffer[1] = 0x00;
	SPS30_write(inst, inst->txBuffer, 2);
	SPS30_read(inst);
}

void SPS30_reset(SPS30* inst)
{
	inst->txBuffer[0] = SPS30_RESET;
	inst->txBuffer[1] = 0x00;
	SPS30_write(inst, inst->txBuffer, 2);
	SPS30_read(inst);
}

bool SPS30_present(SPS30* inst)
{
	inst->txBuffer[0] = SPS30_DEVICE_INFO;
	inst->txBuffer[1] = 0x01;
	inst->txBuffer[2] = 0x01;
	SPS30_write(inst, inst->txBuffer, 3);

	return SPS30_read(inst);
}

void SPS30_clean(SPS30* inst)
{
	inst->txBuffer[0] = SPS30_START_FAN;
	inst->txBuffer[1] = 0x00;
	SPS30_write(inst, inst->txBuffer, 2);
	SPS30_read(inst);
}
