// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//		Transmitter code
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#include "run.h"
#include <stdbool.h>
#include "sx1278.h"
#include "config.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

#define TRANSMITTER_DEBUG 1
#define TRANSMITTER_PRINT_PACKET 1

SX1278 radio;
bool firstTransmission;

static bool transmitter_begin(void)
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
		if (TRANSMITTER_DEBUG) println("[LoRa] Init unsuccessful, retrying...");
		attempts++;

		if (attempts >= 5)
		{
			if (TRANSMITTER_DEBUG) println("[LoRa] Too many attempts, Radio is not active!");
			return false;
		}
	}
	if (TRANSMITTER_DEBUG) println("[LoRa] Radio successful init!");
	firstTransmission = true;
}

static bool transmitter_loop(uint8_t* buf, uint8_t len)
{
	if (radio.active)
	{
		if (radio.useDio0IRQ)
		{
			// manually check for interrupt
			if (firstTransmission || (radio.pendingIRQ && HAL_GPIO_ReadPin(radio.dio0_port, radio.dio0) == GPIO_PIN_SET))
			{
				if (TRANSMITTER_DEBUG) println("[LoRa] Transmission finished.");
				if (!firstTransmission) SX1278_dio0_IRQ(&radio);
				if (TRANSMITTER_PRINT_PACKET)
				{
					printLen = sprintf(printBuffer, "[LoRa] Input packet length: %d, content: [", len);
					printv(printBuffer, printLen);
					printv(buf, len);
					println("]");
				}
				SX1278_transmit(&radio, buf, len);
				firstTransmission = false;
				HAL_GPIO_TogglePin(LEDD_GPIO_Port, LEDD_Pin);
				if (TRANSMITTER_DEBUG) println("[LoRa] Packet pushed!");
				return true;
			}
			else return false;
		}
		else
		{
			SX1278_transmit(&radio, buf, len);
			if (TRANSMITTER_DEBUG) println("[LoRa] Transmission finished.");
			return true;
		}
	}
	return false;
}
