// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//		Receiver code
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#include "run.h"
#include <stdbool.h>
#include "sx1278.h"
#include "config.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

#define RECEIVER_DEBUG 1
#define RECEIVER_PRINT_PACKET 1

SX1278 recradio;
bool firstReception;

static bool receiver_begin(void)
{
	recradio.reset = LR_RESET_Pin;
	recradio.dio0 = LR_DIO0_Pin;
	recradio.nss = LR_NSS_Pin;
	recradio.rxtim = LR_TIM_Pin;
	recradio.reset_port = LR_RESET_GPIO_Port;
	recradio.dio0_port = LR_DIO0_GPIO_Port;
	recradio.nss_port = LR_NSS_GPIO_Port;
	recradio.rxtim_port = LR_TIM_GPIO_Port;
	recradio.spi = Get_SPI1_Instance();
	recradio.config = sx1278_default_config;

	recradio.useDio0IRQ = true;

	uint8_t attempts = 0;

	while (!SX1278_init(&recradio))
	{
		HAL_Delay(500);
		if (RECEIVER_DEBUG) println("[LoRa] Init unsuccessful, retrying...");
		attempts++;

		if (attempts >= 5)
		{
			if (RECEIVER_DEBUG) println("[LoRa] Too many attempts, Radio is not active!");
			return false;
		}
	}
	if (RECEIVER_DEBUG) println("[LoRa] Radio successful init!");
	firstReception = true;
}

static bool receiver_loop()
{
	if (recradio.active)
	{
		if (recradio.useDio0IRQ)
		{
			// manually check for interrupt
			if (firstReception || (recradio.pendingIRQ && HAL_GPIO_ReadPin(recradio.dio0_port, recradio.dio0) == GPIO_PIN_SET))
			{
				if (RECEIVER_DEBUG) println("[LoRa] Packet received!");
				if (!firstReception) SX1278_dio0_IRQ(&recradio);

				if (recradio.newPacket)
				{
					if (RECEIVER_PRINT_PACKET)
					{
						printLen = sprintf(printBuffer, "[LoRa] Packet length: %d, rssi: %d, content: [", recradio.rxLen, recradio.rssi);
						printv(printBuffer, printLen);
						printv(recradio.rxBuffer, recradio.rxLen);
						println("]");
					}
				}
				else if (recradio.rxTimeout)
				{
					println("[LoRa] Receive timeout.");
				}
				SX1278_receive(&recradio);

				firstReception = false;
				HAL_GPIO_TogglePin(LEDC_GPIO_Port, LEDC_Pin);
				if (RECEIVER_DEBUG) println("[LoRa] Waiting for packet...");
				return true && !recradio.rxTimeout;
			}
		}
		else
		{
			SX1278_receive(&recradio);
			if (RECEIVER_DEBUG) println("[LoRa] Packet received.");
			return true;
		}
	}
	return false;
}
