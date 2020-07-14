#ifndef DUPLEX_C
#define DUPLEX_C

#include "run.h"
#include <stdbool.h>
#include "config.h"

#include "sx1278.h"

bool transmitting;
uint8_t packetNumber;

static bool duplex_setup(void)
{
	(*Common.log_print)("*D00"); // [DUPLEX] Hello!
	#if RADIO_ENABLE
		Common.radio.reset = LR_RESET_Pin;		Common.radio.reset_port = LR_RESET_GPIO_Port;
		Common.radio.dio0 = LR_DIO0_Pin;		Common.radio.dio0_port = LR_DIO0_GPIO_Port;
		Common.radio.nss = LR_NSS_Pin;			Common.radio.nss_port = LR_NSS_GPIO_Port;
		Common.radio.spi = Get_SPI1_Instance();
		Common.radio.config = sx1278_default_config;
		Common.radio.useDio0IRQ = true;

		#if DUPLEX_DEBUG
			println("[DUPLEX] Initializing SX1278");
		#endif
		(*Common.log_print)("*D01");

		uint8_t attempts = 0;
		while (!SX1278_init(&(Common.radio)))
		{
			if (++attempts > 5)
			{
				#if DUPLEX_DEBUG
					println("error: [DUPLEX] No connection with SX1278, radio is not active");
				#endif
				(*Common.log_print)("*ED01");
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
		(*Common.log_print)("*D02");
		
		packetNumber = 1;
		transmitting = true;

		SX1278_transmit(&(Common.radio), Common.radio.txBuffer, 22);	// Send empty packet to kickstart duplex
		return true;
	#else // RADIO_ENABLE
		#if DUPLEX_DEBUG
			println("warning: [DUPLEX] RADIO DISABLED!")
		#endif
		(*Common.log_print)("*WD00");
		Common.radio.active = false;
		return false;
	#endif
}

static bool duplex_checkINT(void)
{
	return ((Common.radio.pendingIRQ && HAL_GPIO_ReadPin(Common.radio.dio0_port, Common.radio.dio0) == HIGH) || SX1278_intTimeout(&(Common.radio)));
}

static void decodePacket()
{
	if (Common.radio.newPacket)
	{
		// format: MOT-1/8, SER-1/8, OPMOD-6/8, ANG-1, LAT-4, LON-4, ALT-2
		uint32_t temv = 0;

		temv = (uint32_t)Common.radio.rxBuffer[0];
		Common.motors_enabled = temv & (1 << 7);
		Common.servo_enabled = temv & (1 << 6);
		Common.operation_mode = temv & (0xFF >> 2);

		if (Common.operation_mode == 2 || Common.operation_mode == 3)
		{
			temv = (uint32_t)Common.radio.rxBuffer[1];
			Common.target_yaw = (float)(temv) * 360.0 / 255.0; 
		}
		if (Common.radio.rxLen == 13 && Common.operation_mode == 1)
		{
			bytesToFloat(Common.radio.rxBuffer + 3, (uint8_t*)&(Common.target_lat));
			bytesToFloat(Common.radio.rxBuffer + 7, (uint8_t*)&(Common.target_lat));
			
			memcpy((uint8_t*)&temv, Common.radio.rxBuffer + 11, 2);
			Common.target_alt = (float)(temv) / 10;
		}
	}
}

static void preparePacket()
{
	// format: TEMP-2, PRES-3, LAT-4, LON-4, ALT-2, YAW-1, PITCH-1, ROLL-1, SPS1-1, SPS10-1, OPMODE-1, PN-1
	uint32_t temv = 0;
	uint8_t B = 0;

	temv = (uint32_t)((Common.bmp.temperature + 10) * 1000);
	memcpy(Common.radio.txBuffer + B, (uint8_t*)&temv, 2); B += 2;	// 0:2

	temv = (uint32_t)(Common.bmp.pressure * 10000);
	memcpy(Common.radio.txBuffer + B, (uint8_t*)&temv, 3); B += 3;	// 3:6

	floatToBytes(&(Common.gps.latitudeDegrees), Common.radio.txBuffer + B); B += 4;		// 7:10
	floatToBytes(&(Common.gps.longitudeDegrees), Common.radio.txBuffer + B); B += 4;	// 11:14

	temv = (uint32_t)(Common.gps.altitude * 10);
	memcpy(Common.radio.txBuffer + B, (uint8_t*)&temv, 2); B += 2;	// 15:16

	Common.radio.txBuffer[B] = (uint8_t)(Common.mpu.yaw * (255.0 / 360.0));	B++;	// 17
	Common.radio.txBuffer[B] = (uint8_t)(Common.mpu.pitch * (255.0 / 360.0)); B++;	// 18
	Common.radio.txBuffer[B] = (uint8_t)(Common.mpu.roll * (255.0 / 360.0)); B++;	// 19

	/*
	Common.radio.txBuffer[B] = (uint8_t)(Common.sps.pm1 * (255.0 / __)); B++;		// 20
	Common.radio.txBuffer[B] = (uint8_t)(Common.sps.pm10 * (255.0 / __)); B++;		// 21
	*/
	B+=2;

	Common.radio.txBuffer[B] = Common.operation_mode; B++;	// 22
	Common.radio.txBuffer[B] = packetNumber; B++;			// 23
	Common.radio.txLen = B;
}

static void duplex_loop(void)
{
	#if RADIO_ENABLE
		if (Common.radio.active && Common.radio.useDio0IRQ && duplex_checkINT())	// Manually check for interrupt
		{
			SX1278_dio0_IRQ(&(Common.radio));
			if (!transmitting)	// If in receiving mode: decode packet
			{
				decodePacket();
				#if DUPLEX_DEBUG
					if (Common.radio.rxTimeout)
					{
						println("[DUPLEX] Receive timeout.");
					}
				#endif
			}

			transmitting = true;

			if (packetNumber == DUPLEX_TX_COUNT)
			{
				SX1278_receive(&(Common.radio));
				packetNumber = 0;
				transmitting = false;
				return;
			}
			else
			{
				preparePacket();
				SX1278_transmit(&(Common.radio), &(Common.radio.txBuffer), &(Common.radio.txLen));	// Packet is written directly to memory buffer of the instance
				packetNumber++;
				return;
			}
		}
	#endif
	return;
}

#endif
