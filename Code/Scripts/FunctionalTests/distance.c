// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//		Distance tests code
//		Green LED - Receive
//		Yellow LED - Transmit
//		Short click - mode swap
//		Long click - parameter swap
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#define DISTANCE_DEBUG 1

#include "main.h"
#include "sx1278.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

SX1278 radio;

static bool distance_begin(SX1278_config params)
{
	radio.reset = LR_RESET_Pin;
	radio.dio0 = LR_DIO0_Pin;
	radio.nss = LR_NSS_Pin;
	//radio.rxtim = LR_TIM_Pin;
	radio.reset_port = LR_RESET_GPIO_Port;
	radio.dio0_port = LR_DIO0_GPIO_Port;
	radio.nss_port = LR_NSS_GPIO_Port;
	//radio.rxtim_port = LR_TIM_GPIO_Port;
	radio.spi = Get_SPI1_Instance();
	radio.config = params;

	radio.useDio0IRQ = true;

	uint8_t attempts = 0;

	while (!SX1278_init(&radio))
	{
		HAL_Delay(500);
		if (DISTANCE_DEBUG) println("[LoRa] Init unsuccessful, retrying...");
		attempts++;

		if (attempts >= 5)
		{
			if (DISTANCE_DEBUG) println("[LoRa] Too many attempts, Radio is not active!");
			return false;
		}
	}
	if (DISTANCE_DEBUG) println("[LoRa] Radio successful init!");
}
