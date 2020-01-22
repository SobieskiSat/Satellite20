#include <stdbool.h>
#include "sx1278.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"

// writes byte [data] on SPI bus
void SX1278_write(SPI_HandleTypeDef* spi, uint8_t data)
{
	HAL_SPI_Transmit(spi, &data, 1, 1000);
	while (HAL_SPI_GetState(spi) != HAL_SPI_STATE_READY);
}

void SX1278_command(SX1278* inst, uint8_t addr, uint8_t cmd)
{
	HAL_GPIO_WritePin(inst->nss_port, inst->nss, GPIO_PIN_RESET);

	SX1278_write(inst->spi, addr | 0x80);
	SX1278_write(inst->spi, cmd);

	HAL_GPIO_WritePin(inst->nss_port, inst->nss, GPIO_PIN_SET);
}

void SX1278_command_burst(SX1278* inst, uint8_t addr, uint8_t* buff, uint8_t len)
{
	if (len <= 1) return;

	HAL_GPIO_WritePin(inst->nss_port, inst->nss, GPIO_PIN_RESET);

	SX1278_write(inst->spi, addr | 0x80);
	for (uint8_t i = 0; i < len; i++)
	{
		SX1278_write(inst->spi, *(buff + i));	//explanation: *(buff + i) = buff[i]
	}

	HAL_GPIO_WritePin(inst->nss_port, inst->nss, GPIO_PIN_SET);
}

uint8_t SX1278_read(SPI_HandleTypeDef* spi)
{
	uint8_t txByte = 0x00;
	uint8_t rxByte = 0x00;

	HAL_SPI_TransmitReceive(spi, &txByte, &rxByte, 1, 1000);
	while (HAL_SPI_GetState(spi) != HAL_SPI_STATE_READY);

	return rxByte;
}

uint8_t SX1278_read_address(SX1278* inst, uint8_t addr)
{
	uint8_t recv;

	HAL_GPIO_WritePin(inst->nss_port, inst->nss, GPIO_PIN_RESET);

	SX1278_write(inst->spi, addr);
	recv = SX1278_read(inst->spi);

	HAL_GPIO_WritePin(inst->nss_port, inst->nss, GPIO_PIN_SET);

	return recv;
}

void SX1278_sleep(SX1278* inst)
{
	SX1278_command(inst, LR_RegOpMode, 0x08);
	inst->status = SLEEP;
}

void SX1278_standby(SX1278* inst)
{
	SX1278_command(inst, LR_RegOpMode, 0x09);
	inst->status = STANDBY;
}

bool SX1278_begin(SX1278* inst)
{
	HAL_GPIO_WritePin(inst->nss_port, inst->nss, GPIO_PIN_SET);
	HAL_GPIO_WritePin(inst->reset_port, inst->reset, GPIO_PIN_SET);

	SX1278_sleep(inst);		//Change modem mode Must in Sleep mode
	HAL_Delay(15);

	SX1278_command(inst, LR_RegOpMode, 0x88); // entryLora
	//SX1278_command(inst, 0x5904); //?? Change digital regulator form 1.6V to 1.47V: see errata note

	SX1278_command_burst(inst, LR_RegFrMsb, inst->frequency, 3); //setting  frequency parameter

	//setting base parameter
	SX1278_command(inst, LR_RegPaConfig, inst->power); 			//Setting output power parameter

	SX1278_command(inst, LR_RegOcp, 0x0B);			//RegOcp,Close Ocp
	SX1278_command(inst, LR_RegLna, 0x23);		//RegLNA,High & LNA Enable
	if (inst->spreadingFactor == SX1278_SF_6)
	{
		uint8_t tmp;

		SX1278_command(inst, LR_RegModemConfig1, ((inst->bandWidth << 4) + (inst->codingRate << 1) + 0x01)); //Implicit Enable CRC Enable(0x02) & Error Coding rate 4/5(0x01), 4/6(0x02), 4/7(0x03), 4/8(0x04)
		SX1278_command(inst, LR_RegModemConfig2, ((inst->spreadingFactor << 4) + (inst->crc << 2) + 0x03));

		tmp = SX1278_read_address(inst, 0x31);
		tmp &= 0xF8;
		tmp |= 0x05;
		SX1278_command(inst, 0x31, tmp);
		SX1278_command(inst, 0x37, 0x0C);
	}
	else
	{
		SX1278_command(inst, LR_RegModemConfig1, ((inst->bandWidth << 4) + (inst->codingRate << 1) + 0x00)); //Explicit Enable CRC Enable(0x02) & Error Coding rate 4/5(0x01), 4/6(0x02), 4/7(0x03), 4/8(0x04)
		SX1278_command(inst, LR_RegModemConfig2, ((inst->spreadingFactor << 4) + (inst->crc << 2) + 0x03)); //SFactor &  LNA gain set by the internal AGC loop
	}

	SX1278_command(inst, LR_RegSymbTimeoutLsb, 0xFF); //RegSymbTimeoutLsb Timeout = 0x3FF(Max)
	SX1278_command(inst, LR_RegPreambleMsb, 0x00); //RegPreambleMsb
	SX1278_command(inst, LR_RegPreambleLsb, 12); //RegPreambleLsb 8+4=12byte Preamble
	SX1278_command(inst, REG_LR_DIOMAPPING2, 0x01); //RegDioMapping2 DIO5=00, DIO4=01
	inst->recvLen = 0;
	SX1278_standby(inst); //Entry standby mode

	return true;
}

int SX1278_LoRaEntryTx(SX1278* inst, uint8_t length, uint32_t timeout)
{
	uint8_t addr;
	uint8_t temp;

	inst->packetLength = length;

	SX1278_defaultConfig(inst); //setting base parameter
	SX1278_command(inst, REG_LR_PADAC, 0x87);	//Tx for 20dBm
	SX1278_command(inst, LR_RegHopPeriod, 0x00); //RegHopPeriod NO FHSS
	SX1278_command(inst, REG_LR_DIOMAPPING1, 0x41); //DIO0=01, DIO1=00,DIO2=00, DIO3=01
	SX1278_clearLoRaIrq(inst);
	SX1278_command(inst, LR_RegIrqFlagsMask, 0xF7); //Open TxDone interrupt
	SX1278_command(inst, LR_RegPayloadLength, length); //RegPayloadLength 21byte
	addr = SX1278_read_address(inst, LR_RegFifoTxBaseAddr); //RegFiFoTxBaseAddr
	SX1278_command(inst, LR_RegFifoAddrPtr, addr); //RegFifoAddrPtr

	while (1) {
		temp = SX1278_read_address(inst, LR_RegPayloadLength);
		if (temp == length) {
			inst->status = TX;
			return 1;
		}

		if (--timeout == 0) {
			SX1278_hw_Reset(inst->hw);
			SX1278_defaultConfig(inst);
			return 0;
		}
	}
}
