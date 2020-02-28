// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//		Duplex transmitter code
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#include "run.h"
#include <stdbool.h>
#include "sx1278.h"
#include "config.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "logger.h"

#define DUPLEX_DEBUG 0
#define DUPLEX_PRINT_PACKET 1
#define DUPLEX_TX_COUNT 7

SX1278 radio;
bool isReceiving;
bool firstTransmission;
uint8_t packetNumber;

static bool duplex_begin(void)
{
	radio.reset = LR_RESET_Pin;
	radio.dio0 = LR_DIO0_Pin;
	radio.nss = LR_NSS_Pin;
	radio.rxtim = LR_TIM_Pin;
	radio.reset_port = LR_RESET_GPIO_Port;
	radio.dio0_port = LR_DIO0_GPIO_Port;
	radio.nss_port = LR_NSS_GPIO_Port;
	radio.rxtim_port = LR_TIM_GPIO_Port;
	radio.spi = Get_SPI1_Instance();
	radio.config = sx1278_default_config;

	radio.useDio0IRQ = true;

	uint8_t attempts = 0;

	while (!SX1278_init(&radio))
	{
		HAL_Delay(500);
		if (DUPLEX_DEBUG) println("[LoRa] Init unsuccessful, retrying...");
		attempts++;

		if (attempts >= 5)
		{
			if (DUPLEX_DEBUG) println("[LoRa] Too many attempts, Radio is not active!");
			return false;
		}
	}
	if (DUPLEX_DEBUG) println("[LoRa] Radio successful init!");
	packetNumber = 1;
	firstTransmission = true;
	isReceiving = false;
}

static bool duplex_checkINT()
{
	return (firstTransmission || (radio.pendingIRQ && HAL_GPIO_ReadPin(radio.dio0_port, radio.dio0) == GPIO_PIN_SET) || SX1278_intTimeout(&radio));
}

static bool duplex_loop(uint8_t* buf, uint8_t len, bool externallyInvoked)
{
	if (radio.active)
	{
		if (radio.useDio0IRQ)
		{
			// manually check for interrupt
			if (externallyInvoked || duplex_checkINT())
			{
				if (!firstTransmission) SX1278_dio0_IRQ(&radio);
				if (packetNumber == 0)
				{
					if (DUPLEX_DEBUG) println("[LoRa] Packet received!");
					if (radio.newPacket)
					{
						log_radio(&radio, false);
						if (DUPLEX_PRINT_PACKET)
						{
							printLen = sprintf(printBuffer, "[LoRa] Packet length: %d, rssi: %d, content: [", radio.rxLen, radio.rssi);
							printv(printBuffer, printLen);
							printv((char*)radio.rxBuffer, radio.rxLen);
							println("]");
						}
					}
					else if (radio.rxTimeout)
					{
						println("[LoRa] Receive timeout.");
					}
					HAL_GPIO_WritePin(LEDC_GPIO_Port, LEDC_Pin, GPIO_PIN_RESET);
					isReceiving = false;
				}
				else
				{
					if (DUPLEX_DEBUG) println("[LoRa] Transmission finished.");
					if (DUPLEX_PRINT_PACKET)
					{
						printLen = sprintf(printBuffer, "[LoRa] Input packet length: %d, content: [", len);
						printv(printBuffer, printLen);
						printv(buf, len);
						println("]");
					}
					log_radio(&radio, true);
				}

				if (packetNumber == DUPLEX_TX_COUNT - 1)
				{
					SX1278_receive(&radio);
					packetNumber = -1;
					isReceiving = true;
					HAL_GPIO_WritePin(LEDC_GPIO_Port, LEDC_Pin, GPIO_PIN_SET);
				}
				else
				{
					buf[len - 1] = packetNumber;
					SX1278_transmit(&radio, buf, len);
					if (DUPLEX_DEBUG) println("[LoRa] Packet pushed!");
				}
				firstTransmission = false;
				packetNumber++;
				HAL_GPIO_TogglePin(LEDD_GPIO_Port, LEDD_Pin);
				return true && !(radio.rxTimeout && isReceiving);
			}
			else return false;
		}
		else
		{
			SX1278_transmit(&radio, buf, len);
			if (DUPLEX_DEBUG) println("[LoRa] Transmission finished.");
			return true;
		}
	}
	return false;
}
