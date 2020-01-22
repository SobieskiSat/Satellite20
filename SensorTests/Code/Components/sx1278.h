#ifndef COMPONENTS_SX1278_H_
#define COMPONENTS_SX1278_H_

#include "stm32f4xx_hal.h"

#define SX1278_MAX_PACKET	256

typedef enum _SX1278_STATUS {
	SLEEP, STANDBY, TX, RX
} SX1278_Status;

typedef struct
{
	uint16_t frequency;
	uint8_t power;
	uint8_t spreadingFactor;
	uint8_t codingRate;
	uint8_t bandWidth;
	uint8_t crc;
} SX1278_config;

typedef struct
{
	uint8_t frequency[3];
	uint8_t power;
	uint8_t spreadingFactor;
	uint8_t codingRate;
	uint8_t bandWidth;

	uint8_t crc;

	uint8_t packetLength;

	SX1278_Status status;

	uint16_t reset;
	uint16_t dio0;
	uint16_t nss;
	GPIO_TypeDef* reset_port;
	GPIO_TypeDef* dio0_port;
	GPIO_TypeDef* nss_port;
	SPI_HandleTypeDef* spi;

	uint8_t rxBuffer[SX1278_MAX_PACKET];
	uint8_t recvLen;
} SX1278;

bool SX1278_begin(SX1278* inst);

#define SX1278_DEFAULT_TIMEOUT	3000

#define SX1278_CR_4_5	0x01
#define SX1278_CR_4_6 	0x02
#define SX1278_CR_4_7 	0x03
#define SX1278_CR_4_8	0x04

#define SX1278_CRC_DIS	0x00
#define SX1278_CRC_EN	0x01

#define SX1278_POWER_20DBM		0xFF
#define SX1278_POWER_17DBM		0xFC
#define SX1278_POWER_14DBM		0xF9
#define SX1278_POWER_11DBM		0xF6

#define SX1278_SF_6		6
#define SX1278_SF_7		7
#define SX1278_SF_8		8
#define SX1278_SF_9		9
#define SX1278_SF_10		10
#define SX1278_SF_11		11
#define SX1278_SF_12		12

#define	SX1278_BW_7_8KHZ	0
#define	SX1278_BW_10_4KHZ	1
#define	SX1278_BW_15_6KHZ	2
#define	SX1278_BW_20_8KHZ	3
#define	SX1278_BW_31_2KHZ	4
#define	SX1278_BW_41_7KHZ	5
#define	SX1278_BW_62_5KHZ	6
#define	SX1278_BW_125KHZ	7
#define	SX1278_BW_250KHZ	8
#define	SX1278_BW_500KHZ	9

#define LR_RegFifo                                  0x00
// Common settings
#define LR_RegOpMode                                0x01
#define LR_RegFrMsb                                 0x06
#define LR_RegFrMid                                 0x07
#define LR_RegFrLsb                                 0x08
// Tx settings
#define LR_RegPaConfig                              0x09
#define LR_RegPaRamp                                0x0A
#define LR_RegOcp                                   0x0B
// Rx settings
#define LR_RegLna                                   0x0C
// LoRa registers
#define LR_RegFifoAddrPtr                           0x0D
#define LR_RegFifoTxBaseAddr                        0x0E
#define LR_RegFifoRxBaseAddr                        0x0F
#define LR_RegFifoRxCurrentaddr                     0x10
#define LR_RegIrqFlagsMask                          0x11
#define LR_RegIrqFlags                              0x12
#define LR_RegRxNbBytes                             0x13
#define LR_RegRxHeaderCntValueMsb                   0x14
#define LR_RegRxHeaderCntValueLsb                   0x15
#define LR_RegRxPacketCntValueMsb                   0x16
#define LR_RegRxPacketCntValueLsb                   0x17
#define LR_RegModemStat                             0x18
#define LR_RegPktSnrValue                           0x19
#define LR_RegPktRssiValue                          0x1A
#define LR_RegRssiValue                             0x1B
#define LR_RegHopChannel                            0x1C
#define LR_RegModemConfig1                          0x1D
#define LR_RegModemConfig2                          0x1E
#define LR_RegSymbTimeoutLsb                        0x1F
#define LR_RegPreambleMsb                           0x20
#define LR_RegPreambleLsb                           0x21
#define LR_RegPayloadLength                         0x22
#define LR_RegMaxPayloadLength                      0x23
#define LR_RegHopPeriod                             0x24
#define LR_RegFifoRxByteAddr                        0x25
// I/O settings
#define REG_LR_DIOMAPPING1                          0x40
#define REG_LR_DIOMAPPING2                          0x41
// Version
#define REG_LR_VERSION                              0x42
// Additional settings
#define REG_LR_PLLHOP                               0x44
#define REG_LR_TCXO                                 0x4B
#define REG_LR_PADAC                                0x4D
#define REG_LR_FORMERTEMP                           0x5B
#define REG_LR_AGCREF                               0x61
#define REG_LR_AGCTHRESH1                           0x62
#define REG_LR_AGCTHRESH2                           0x63
#define REG_LR_AGCTHRESH3                           0x64

#endif /* COMPONENTS_SX1278_H_ */
