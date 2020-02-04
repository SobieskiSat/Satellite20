#include "sx1278.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"
#include "run.h"

//#### SPI communication with SX1278 ####

void SX1278_write(SPI_HandleTypeDef* spi, uint8_t data)
{
	// writes byte [data] on SPI bus
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
	uint8_t i;

	if (len <= 1) return;

	HAL_GPIO_WritePin(inst->nss_port, inst->nss, GPIO_PIN_RESET);

	SX1278_write(inst->spi, addr | 0x80);
	for (i = 0; i < len; i++)
	{
		SX1278_write(inst->spi, *(buff + i));	//explanation: *(buff + i) = buff[i]
	}

	HAL_GPIO_WritePin(inst->nss_port, inst->nss, GPIO_PIN_SET);
}

uint8_t SX1278_read(SPI_HandleTypeDef* spi)
{
	// reads byte from SPI bus
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
void SX1278_read_burst(SX1278* inst, uint8_t addr, uint8_t* buff, uint8_t len)
{
	uint8_t i;

	if (len <= 1) return;

	HAL_GPIO_WritePin(inst->nss_port, inst->nss, GPIO_PIN_RESET);

	SX1278_write(inst->spi, addr);
	for (i = 0; i < len; i++)
	{
		*(buff + i) = SX1278_read(inst->spi);	//explanation: *(buff + i) = buff[i]
	}

	HAL_GPIO_WritePin(inst->nss_port, inst->nss, GPIO_PIN_SET);
}

//#### SX1278 usage functions ####

bool SX1278_init(SX1278* inst)
{
	//default pin configuration
	HAL_GPIO_WritePin(inst->nss_port, inst->nss, GPIO_PIN_SET);
	HAL_GPIO_WritePin(inst->reset_port, inst->reset, GPIO_PIN_SET);

	//unable to establish the connection with module
	if (SX1278_read_address(inst, REG_LR_VERSION) != 0x12) return false;

	//changes must be performed in a Sleep mode
	SX1278_sleep(inst);

	//HAL_Delay(15);

	//0x88 = 1_00_0_1_000 = Lora, FSK, (r), LowFreq, Sleep
	SX1278_command(inst, LR_RegOpMode, 0x88);

	//SX1278_command(inst, 0x5904); //?? Change digital regulator form 1.6V to 1.47V: see errata note

	//Setting three frequency bytes
	//####################################
	//[!!!!] heavy low level shit going out there, must check if correct
	//####################################
	float step_in_mhz = 32/pow(2, 19);
	float multiplier = inst->config.frequency/step_in_mhz;
	uint8_t multiplierBytes[4] = {0};
	floatToBytes(multiplier, multiplierBytes);
	// start from 2nd byte, dependent on byteorder, must check if correct
	SX1278_command(inst, LR_RegFrMsb, multiplierBytes[1]);
	SX1278_command(inst, LR_RegFrMid, multiplierBytes[2]);
	SX1278_command(inst, LR_RegFrLsb, multiplierBytes[3]);

	SX1278_command(inst, LR_RegPaConfig, inst->config.power);				//Setting transmit power
	SX1278_command(inst, LR_RegOcp, 0x2B);			// [was 0x0B] Over current protection set to 100mA
	SX1278_command(inst, LR_RegLna, 0x23);			// LNA settings: G1 - max gain, Boost on
	SX1278_command(inst, REG_LR_PADAC, 0x87);		//high power setting +20dBm
	SX1278_command(inst, LR_RegHopPeriod, 0x00);	//frequency hopping off

	if (inst->config.spreadingFactor == SX1278_SF_6)
	{
		uint8_t tmp;
		//implicit CRC enable
		SX1278_command(inst, LR_RegModemConfig1, ((inst->config.bandWidth << 4) + (inst->config.codingRate << 1) + 0x01));
		SX1278_command(inst, LR_RegModemConfig2, ((inst->config.spreadingFactor << 4) + (inst->config.crc << 2) + (uint8_t)(inst->config.rxTimeoutSymb >> 8)));
		tmp = SX1278_read_address(inst, 0x31);
		tmp &= 0xF8;
		tmp |= 0x05;
		SX1278_command(inst, 0x31, tmp);
		SX1278_command(inst, 0x37, 0x0C);
	}
	else
	{
		//explicit CRC enable
		SX1278_command(inst, LR_RegModemConfig1, ((inst->config.bandWidth << 4) + (inst->config.codingRate << 1) + 0x00));
		//SFactor &  LNA gain set by the internal AGC loop
		SX1278_command(inst, LR_RegModemConfig2, ((inst->config.spreadingFactor << 4) + (inst->config.crc << 2) + (uint8_t)(inst->config.rxTimeoutSymb >> 8)));
	}

	SX1278_command(inst, LR_RegSymbTimeoutLsb, (uint8_t)(inst->config.rxTimeoutSymb & 0x00FF));	//recievier timeout value [timeout = symbtimeout*ts]
	SX1278_command(inst, LR_RegPreambleMsb, 0x00);		//Setting the preable length?
	SX1278_command(inst, LR_RegPreambleLsb, 12);		//8+4=12byte Preamble
	SX1278_command(inst, REG_LR_DIOMAPPING2, 0x01);		//RegDioMapping2 DIO5=00, DIO4=01

	inst->newPacket = true;
	inst->rxTimeout = false;
	inst->rxDone = false;
	inst->crcError = false;
	inst->rssi = 1;
	inst->newPacket = false;
	inst->pendingIRQ = false;
	inst->txLen = 0;
	inst->rxLen = 0;

	SX1278_standby(inst);

	return true;
}

bool SX1278_transmit(SX1278* inst, uint8_t* txBuffer, uint8_t length)
{
	// check if the module is ready for the transmission
	if (inst->mode == STANDBY)
	{
		SX1278_tx_mode(inst);
		SX1278_tx_input(inst, txBuffer, length);
		SX1278_tx_push(inst);

		if (inst->useDio0IRQ)
		{
			//waiting for interrupt
			inst->pendingIRQ = true;
		}
		else
		{
			//wait for dio0 pin to rise
			while (HAL_GPIO_ReadPin(inst->dio0_port, inst->dio0) == GPIO_PIN_RESET);

			return SX1278_tx_finish(inst);
		}

		return true;
	}
	else return false;
}

bool SX1278_receive(SX1278* inst)
{
	// [!!] writes data to the inst->rxBuffer
	if (inst->mode == STANDBY)
	{
		SX1278_rx_mode(inst);

		if (inst->useDio0IRQ)
		{
			//waiting for interrupt
			inst->pendingIRQ = true;
		}
		else
		{
			//wait for dio0 pin to rise
			while (HAL_GPIO_ReadPin(inst->dio0_port, inst->dio0) == GPIO_PIN_RESET);

			SX1278_rx_get_packet(inst);
			return inst->newPacket;
		}

		return true;
	}
	else return false;
}

//#### Data send / receive routines ####

void SX1278_tx_input(SX1278* inst, uint8_t* txBuffer, uint8_t length)
{
	SX1278_command(inst, LR_RegPayloadLength, length);	//(this register must difine when the data long of one byte in SF is 6)
	SX1278_command_burst(inst, 0x00, txBuffer, length);

	inst->txLen = length;
}
void SX1278_tx_push(SX1278* inst)
{
	//set module to TX mode and transmit
	SX1278_command(inst, LR_RegOpMode, 0x8b);
}
bool SX1278_tx_finish(SX1278* inst)
{
	inst->irqStatus = SX1278_read_address(inst, LR_RegIrqFlags);
	inst->txDone = ((inst->irqStatus & IRQ_LR_TXDONE) > 0x00);
	SX1278_clearLoRaIrq(inst);
	SX1278_standby(inst);
	return true;
}

bool SX1278_rx_get_packet(SX1278* inst)
{
	uint8_t addr;
	uint8_t packet_size;

	SX1278_update_IRQ_status(inst);

	memset(inst->rxBuffer, 0x00, SX1278_MAX_PACKET);	//clear rxBuffer

	addr = SX1278_read_address(inst, LR_RegFifoRxCurrentaddr);	//get last packet address
	SX1278_command(inst, LR_RegFifoAddrPtr, addr);				//set fifo pointer to this address

	//When SpreadingFactor = 6, use Implicit Header mode (Excluding internal packet length)
	if (inst->config.spreadingFactor == SX1278_SF_6)
	{
		// ??? not sure what to put here
		packet_size = inst->txLen;
	}
	else
	{
		packet_size = SX1278_read_address(inst, LR_RegRxNbBytes); //get the number of received bytes
	}

	SX1278_read_burst(inst, 0x00, inst->rxBuffer, packet_size);

	inst->newPacket = (inst->rxTimeout || (inst->crcError && LR_VALIDATE_CRCERROR));
	inst->rssi = SX1278_getRSSI(inst);
	inst->rxLen = packet_size;
	SX1278_clearLoRaIrq(inst);
	SX1278_standby(inst);

	return inst->newPacket;
}

//#### Functions to change SX1278 operation mode ####

void SX1278_tx_mode(SX1278* inst)
{
	uint8_t addr;

	SX1278_clearLoRaIrq(inst);
	SX1278_command(inst, REG_LR_DIOMAPPING1, 0x41);	//DIO0=01, DIO1=00, DIO2=00, DIO3=01
	SX1278_command(inst, LR_RegIrqFlagsMask, 0xF7);	//Open TxDone interrupt

	addr = SX1278_read_address(inst, LR_RegFifoTxBaseAddr);	//read tx_fifo beginning adress in memory
	SX1278_command(inst, LR_RegFifoAddrPtr, addr);			//set fifo pointer there

	inst->txLen = 0;
	inst->mode = TX;
}

void SX1278_rx_mode(SX1278* inst)
{
	uint8_t addr;

	SX1278_clearLoRaIrq(inst);
	SX1278_command(inst, REG_LR_DIOMAPPING1, 0x01);	//DIO=00, DIO1=00,DIO2=00, DIO3=01
	SX1278_command(inst, LR_RegIrqFlagsMask, 0x1F);	//Open RxDone, RxTimeout, crcError interrupt

	addr = SX1278_read_address(inst, LR_RegFifoRxBaseAddr);	//read rx_fifo beginning adress in memory
	SX1278_command(inst, LR_RegFifoAddrPtr, addr);			//set fifo pointer there

	SX1278_command(inst, LR_RegOpMode, 0x8d);				//0x8d = 1_00_0_1_101 = Lora, FSK, (r), Low Frequency mode, Rx mode

	inst->crcError = false;
	inst->rxLen = 0;
	inst->mode = RX;
}

void SX1278_sleep(SX1278* inst)
{
	SX1278_command(inst, LR_RegOpMode, 0x08);
	inst->mode = SLEEP;
}

void SX1278_standby(SX1278* inst)
{
	SX1278_command(inst, LR_RegOpMode, 0x09);
	inst->mode = STANDBY;
}

void SX1278_reset(SX1278* inst) {
	HAL_GPIO_WritePin(inst->nss_port, inst->nss, GPIO_PIN_SET);
	HAL_GPIO_WritePin(inst->reset_port, inst->reset, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(inst->reset_port, inst->reset, GPIO_PIN_SET);
	HAL_Delay(100);
}

//#### Interrupt methods ####

bool SX1278_dio0_IRQ(SX1278* inst)
{
	//function to call when dio0 rises, used only when waiting for the DIO0 interrupt
	//finishes transmit and receive routines
	if (inst->useDio0IRQ && inst->pendingIRQ)
	{
		if (inst->mode == TX)
		{
			return SX1278_tx_finish(inst);
		}
		else if (inst->mode == RX)
		{
			return SX1278_rx_get_packet(inst);
		}
		else
		{
			SX1278_clearLoRaIrq(inst);
			SX1278_standby(inst);
			return false;
		}

		inst->pendingIRQ = false;
		return true;
	}
	else return false;
}

void SX1278_update_IRQ_status(SX1278* inst)
{
	inst->irqStatus = SX1278_read_address(inst, LR_RegIrqFlags);
	inst->rxTimeout = ((inst->irqStatus & IRQ_LR_RXTIMEOUT) > 0x00);
	inst->rxDone = 	  ((inst->irqStatus & IRQ_LR_RXDONE) > 0x00);
	inst->crcError =  ((inst->irqStatus & IRQ_LR_CRCERROR) > 0x00);
}

void SX1278_clearLoRaIrq(SX1278* inst)
{
	SX1278_command(inst, LR_RegIrqFlags, 0xFF);
}

int SX1278_getRSSI(SX1278* inst)
{
  return (-164 + SX1278_read_address(inst, LR_RegPktRssiValue));
}
