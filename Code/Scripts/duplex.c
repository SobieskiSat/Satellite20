#include "run.h"
#include <stdbool.h>
#include "config.h"

#include "sx1278.h"

#define DUPLEX_TX_COUNT 7

bool transmitting;
uint8_t packetNumber;

static bool duplex_setup(void)
{
	(*Common.log_print)("[DUPLEX] Hello!");
	#if RADIO_ENABLE
		radio.reset = LR_RESET_Pin;		radio.reset_port = LR_RESET_GPIO_Port;
		radio.dio0 = LR_DIO0_Pin;		radio.dio0_port = LR_DIO0_GPIO_Port;
		radio.nss = LR_NSS_Pin;			radio.nss_port = LR_NSS_GPIO_Port;
		radio.spi = Get_SPI1_Instance();
		radio.config = sx1278_default_config;
		radio.useDio0IRQ = true;

		#if DUPLEX_DEBUG
			println("[DUPLEX] Initializing SX1278");
		#endif
		(*Common.log_print)("[DUPLEX] Initializing SX1278")

		uint8_t attempts = 0;
		while (!SX1278_init(&(Common.radio)))
		{
			if (++attempts > 5)
			{
				#if DUPLEX_DEBUG
					println("error: [DUPLEX] No connection with SX1278, radio is not active");
				#endif
				(*Common.log_print)("error: [DUPLEX] No connection with SX1278, radio is not active");
				return false;
			}

			#if DUPLEX_DEBUG
				println("[DUPLEX] Init unsuccessful, retrying...");
			#endif
			delay(500);
		}
		#if DUPLEX_DEBUG
			println("[DUPLEX] Successful radio init");
		#endif
		(*Common.log_print)("[DUPLEX] Successful radio init!");
		
		packetNumber = 1;
		firstTransmission = true;
		isReceiving = false;

		SX1278_transmit(&(Common.radio), Common.radio.txBuffer, 22);	// Send empty packet to kickstart duplex
		return true;
	#else // RADIO_ENABLE
		#if DUPLEX_DEBUG
			println("warning: [DUPLEX] RADIO DISABLED!")
		#endif
		(*Common.log_print)("warning: [DUPLEX] RADIO DISABLED!")
		Common.radio.active = false;
		return false;
	#endif
}

static bool duplex_checkINT(void)
{
	return ((Common.radio.pendingIRQ && readPin(radio.dio0) == GPIO_PIN_SET) || SX1278_intTimeout(&(Common.radio)));
}

static void decodePacket()
{
	if (Common.radio.newPacket)
	{

	}
}

static void preparePacket()
{
	uint32_t temv = 0;

	temv = (uint32_t)(bmp.temperature * 10);
	radio.txBuffer[0] = (uint8_t)(temv >> 0) & 0xFF;
	radio.txBuffer[1] = (uint8_t)(temv >> 8) & 0xFF;
	radio.txBuffer[2] = (uint8_t)(temv >> 16) & 0xFF;

	temv = (uint32_t)(bmp.pressure * 10);
	radio.txBuffer[0] = (uint8_t)(temv >> 0) & 0xFF;
	radio.txBuffer[1] = (uint8_t)(temv >> 8) & 0xFF;
	radio.txBuffer[2] = (uint8_t)(temv >> 16) & 0xFF;
	radio.txBuffer[3] = (uint8_t)(temv >> 24) & 0xFF;


	temv = (uint32_t)(gps.latitudeDegrees * 10000000);
	radio.txBuffer[8] = (uint8_t)(temv >> 0) & 0xFF;
	radio.txBuffer[9] = (uint8_t)(temv >> 8) & 0xFF;
	radio.txBuffer[10] = (uint8_t)(temv >> 16) & 0xFF;
	radio.txBuffer[11] = (uint8_t)(temv >> 24) & 0xFF;

	temv = (uint32_t)(gps.longitudeDegrees * 10000000);
	radio.txBuffer[12] = (uint8_t)(temv >> 0) & 0xFF;
	radio.txBuffer[13] = (uint8_t)(temv >> 8) & 0xFF;
	radio.txBuffer[14] = (uint8_t)(temv >> 16) & 0xFF;
	radio.txBuffer[15] = (uint8_t)(temv >> 24) & 0xFF;

	radio.txBuffer[16] = (uint8_t)(mpu.yaw * (255.0 / 360.0));
	radio.txBuffer[17] = (uint8_t)(mpu.pitch * (255.0 / 360.0));
	radio.txBuffer[18] = (uint8_t)(mpu.roll * (255.0 / 360.0));

	radio.txBuffer[19] = 0x00;
	radio.txLen = 20;
}

static bool duplex_loop(void)
{
	#if RADIO_ENABLE
		if (Common.radio.active && Common.radio.useDio0IRQ && duplex_checkINT())	// Manually check for interrupt
		{
			SX1278_dio0_IRQ(&radio);
			if (!transmitting)	// If in receiving mode: decode packet
			{
				decodePacket();
				#if DUPLEX_DEBUG
					if (radio.rxTimeout)
					{
						println("[DUPLEX] Receive timeout.");
					}
				#endif
			}

			transmitting = true;

			if (packetNumber == DUPLEX_TX_COUNT - 1)
			{
				SX1278_receive(&radio);
				packetNumber = 0;
				transmitting = false;
				return !Common.radio.rxTimeout;
			}
			else
			{
				preparePacket();
				SX1278_transmit(&(Common.radio), &(Common.radio.txBuffer), &(Common.radio.txLen));	// Packet is written directly to memory buffer of the instance
				packetNumber++
			}
		}
		else return false;
	#endif
	return false;
}
