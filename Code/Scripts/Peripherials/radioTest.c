// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//		A code to test SX1278
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdbool.h>

#include "run.h"
#include "sx1278.h"
#include "clock.h"
#include "config.h"

SX1278 radio;
uint8_t sendBuffer[SX1278_MAX_PACKET];
uint8_t message_length;
bool newIRQ;
bool justStarted;


uint32_t click;
bool isClicked;
bool flippo;

static bool radioTest_begin(void)
{
	println("[LoRa] Joining the server...");
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

	radio.useDio0IRQ = true; println("[LoRa] I am using DIO0 interrupt.");
	//radio.useDio0IRQ = false; println("[LoRa] I am not using DIO0 interrupt.");

	uint8_t attempts = 0;

	while (!SX1278_init(&radio))
	{
		println("Player [LoRa] could not join the server!");
		attempts++;
		if (attempts >= 5)
		{
			println("[LoRa] Too many attempts, aborting...");
			return false;
		}
	}

	radio.txDone = true;
	radio.rxDone = true;
	radio.newPacket = false;
	newIRQ = false;

	justStarted = true;
	isClicked = false;
	flippo = true;

	return true;
}


static void radioTest_receive(void)
{
	uint8_t i;
	if (!radio.useDio0IRQ)
	{
		if (SX1278_receive(&radio))
		{
			HAL_GPIO_TogglePin(LEDA_GPIO_Port, LEDA_Pin);
			printLen = sprintf(printBuffer, "[LoRa] Valid packet received! Bytes: %d, Rssi: %d, Data:\n\r", radio.rxLen, radio.rssi);
			printv(printBuffer, printLen);

			println("");
			printLen = sprintf(printBuffer, "%d\t%d\t%d\r\n", radio.rxBuffer[0], radio.rxBuffer[1], radio.rxBuffer[2]);
			//printv(printBuffer, printLen);
			while (CDC_Transmit_FS((uint8_t*) printBuffer, printLen) == USBD_BUSY);
			println("");

			/*
			for (i = 0; i < radio.rxLen; i++)
			{
				char character[1] = {0};
				character[0] = (char)radio.rxBuffer[i];
				print(character);
			}
			println("");
*/
			// drive motors with values received from radio
			setMotors((float)radio.rxBuffer[0] / 255, (float)radio.rxBuffer[1] / 255);

			if (radio.rxBuffer[2] == 0xAA) TIM3->CCR3 = 510;
			else if (radio.rxBuffer[2] == 0x55)
			{
				TIM3->CCR3 = 990;
			}

			radio.newPacket = false;
		}
		else
		{
			haltMotors();
		}
	}
}
static void radioTest_transmit(void)
{
	uint8_t i;
	if (!radio.useDio0IRQ)
	{
		memset(sendBuffer, 0x00, SX1278_MAX_PACKET);
		message_length = 3;
		sendBuffer[0] = 0x45;
		sendBuffer[1] = 0x4C;
		sendBuffer[2] = 0x4F;
 		SX1278_transmit(&radio, sendBuffer, message_length);
 		println("[LoRa] PACKET SENT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	}
	else if (radio.useDio0IRQ)
	{
		if ((radio.pendingIRQ && HAL_GPIO_ReadPin(radio.dio0_port, radio.dio0) == GPIO_PIN_SET) || justStarted)
		{
			HAL_GPIO_TogglePin(LEDD_GPIO_Port, LEDD_Pin);
			justStarted = false;
			SX1278_tx_finish(&radio);
			radio.pendingIRQ = false;

			/*
			memset(sendBuffer, 0x00, SX1278_MAX_PACKET);
			message_length = 4 * 4; // 4 floaty
			uint32_t temp = 0;
			uint32_t tempmem[4] = {0};
			temp = (uint32_t)(pressure * 1000000.0);
			memcpy(tempmem, &temp, 8);
			sendBuffer[0] = tempmem[0];
			sendBuffer[1] = tempmem[1];
			sendBuffer[2] = tempmem[2];
			sendBuffer[3] = tempmem[3];

			uint32_t temp1 = 0;
			uint32_t tempmem1[4] = {0};
			temp1 = (uint32_t)(temperature * 1000000.0);
			memcpy(tempmem1, &temp1, 8);
			sendBuffer[4] = tempmem1[0];
			sendBuffer[5] = tempmem1[1];
			sendBuffer[6] = tempmem1[2];
			sendBuffer[7] = tempmem1[3];

			uint32_t temp2 = 0;
			uint32_t tempmem2[4] = {0};
			temp2 = (uint32_t)(gps.longitudeDegrees * 100000.0);
			memcpy(tempmem2, &temp2, 8);
			sendBuffer[8] = tempmem2[0];
			sendBuffer[9] = tempmem2[1];
			sendBuffer[10] = tempmem2[2];
			sendBuffer[11] = tempmem2[3];

			uint32_t temp3 = 0;
			uint32_t tempmem3[4] = {0};
			temp3 = (uint32_t)(gps.latitudeDegrees * 100000.0);
			memcpy(tempmem3, &temp3, 8);
			sendBuffer[12] = tempmem3[0];
			sendBuffer[13] = tempmem3[1];
			sendBuffer[14] = tempmem3[2];
			sendBuffer[15] = tempmem3[3];
*/

			/*
		 	print("Pressure: "); print_float(pressure); println(" ");
		 	print("Temperature: "); print_float(temperature); println(" ");
		 	print("Latitude: "); print_float(lato); println(" ");
		 	print("Lonfitude: "); print_float(lono); println(" ");
			message_length = sprintf(sendBuffer, "%f_%f_%f_%f", pressure, temperature, lato, lono);
			println(sendBuffer);
			*/
			SX1278_transmit(&radio, sendBuffer, message_length);
		 	println("[LoRa] PACKET SENT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
		}
	}
}

static void dio0_IRQ(void)
{
	if (radio.pendingIRQ)
	{
		println("[LoRa] DIO0 interrupt received.");
		newIRQ = true;
		//SX1278_dio0_IRQ(&radio);
		//radio_procedure();
	}
	else
	{
		println("[LoRa] DIO0 interrupt received but NOT used!");
	}
}
