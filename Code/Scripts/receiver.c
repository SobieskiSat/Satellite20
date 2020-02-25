// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//		Receiver code
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#include "run.h"
#include "sx1278.h"
#include "config.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

#define RECEIVER_DEBUG 1
#define RECEIVER_PRINT_PACKET 1

SX1278 radio;
bool firstReception;

static bool receiver_begin(void)
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

static void receiver_loop()
{
	if (radio.active)
	{
		if (radio.useDio0IRQ)
		{
			// manually check for interrupt
			if (firstReception || (radio.pendingIRQ && HAL_GPIO_ReadPin(radio.dio0_port, radio.dio0) == GPIO_PIN_SET))
			{
				if (RECEIVER_DEBUG) println("[LoRa] Packet received!");
				if (!firstReception) SX1278_dio0_IRQ(&radio);

				if (radio.newPacket)
				{
					if (RECEIVER_PRINT_PACKET)
					{
						printLen = sprintf(printBuffer, "[LoRa] Packet length: %d, rssi: %d, content: [", radio.rxLen, radio.rssi);
						printv(printBuffer, printLen);
						printv(radio.rxBuffer, radio.rxLen);
						println("]");
					}
				}
				else if (radio.rxTimeout)
				{
					println("[LoRa] Receive timeout.");
				}
				SX1278_receive(&radio);

				firstReception = false;
				HAL_GPIO_TogglePin(LEDC_GPIO_Port, LEDC_Pin);
				if (RECEIVER_DEBUG) println("[LoRa] Waiting for packet...");
			}
		}
		else
		{
			SX1278_receive(&radio);
			if (RECEIVER_DEBUG) println("[LoRa] Packet received.");
		}
	}
}
