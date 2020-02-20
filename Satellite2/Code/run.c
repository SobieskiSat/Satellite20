// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// 		Main file of the user code
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#include "run.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#include "stm32f4xx_hal_gpio.h"

#include "bmp280.h"
#include "sx1278.h"
#include "sd.h"
#include "motors.h"
#include "clock.h"
#include "config.h"
#include "gps.h"
#include "mpu9250.h"

BMP280 bmp280;
float temperature;
float pressure;

SX1278 radio;
uint8_t sendBuffer[SX1278_MAX_PACKET];
uint8_t message_length;

GPS gps;

bool newIRQ;
uint32_t timenow;

float lato, lono;

uint32_t click;
bool isClicked;
bool flippo;

bool justStarted;

// Servo : TIM3->CCR3 (500;1000)

static void setup(void)
{
	while (HAL_GPIO_ReadPin(BTN_USR_GPIO_Port, BTN_USR_Pin) == GPIO_PIN_SET);

	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_SET);
	println("Hello world!!");	HAL_Delay(500);
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_RESET);

	//if (sd_begin()) println("[SD] joined the server!");
	enableMotors(); println("[MOT] joined the server!");

	/*
	if (radio_begin()) println("[LoRa] joined the server!");
	if (gps_begin()) println("[GPS] joined the server!");
	if (bmp280_begin()) println("[BMP] joined the server!");

	justStarted = true;
	isClicked = false;
	flippo = true;

	TIM3->CCR4 = 990;
	TIM3->CCR3 = 990;


	  // Start servo1 timer
	  HAL_TIM_PWM_Start(Get_TIM3_Instance(), TIM_CHANNEL_3);
	  HAL_TIM_PWM_Start(Get_TIM3_Instance(), TIM_CHANNEL_4);
	//ser1.ccr = &(TIM3->CCR3);
	 */
}

static void loop(void)
{
	//if (sd_begin()) println("[SD] joined the server!");
	//gps_printData();
	//radio_receive();
	//mot_up_down();



	/*
	bmp_getData();
	gps_getData();
	//gps_printData();
	radio_transmit();

	if (HAL_GPIO_ReadPin(BTN_USR_GPIO_Port, BTN_USR_Pin) == GPIO_PIN_RESET && !isClicked)
	{
		HAL_GPIO_WritePin(LEDC_GPIO_Port, LEDC_Pin, GPIO_PIN_SET);
		isClicked = true;
		flippo = false;
		click = millis();
	}

	if (millis() - click >= 1000 * 45 && !flippo)
	{
		flippo = true;
		TIM3->CCR3 = 500;
		TIM3->CCR4 = 500;
		HAL_GPIO_WritePin(LEDC_GPIO_Port, LEDC_Pin, GPIO_PIN_RESET);
	}
*/
}




static void radio_receive(void)
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
static void radio_transmit(void)
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

		 	print("Pressure: "); print_float(pressure); println(" ");
		 	print("Temperature: "); print_float(temperature); println(" ");
		 	print("Latitude: "); print_float(lato); println(" ");
		 	print("Lonfitude: "); print_float(lono); println(" ");
			message_length = sprintf(sendBuffer, "%f_%f_%f_%f", pressure, temperature, lato, lono);
			println(sendBuffer);
			SX1278_transmit(&radio, sendBuffer, message_length);
		 	println("[LoRa] PACKET SENT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
		}
	}
}


static void gps_getData(void)
{
	char c = GPS_read(&gps);
	if (GPS_newNMEAreceived(&gps))
	{
		if (GPS_parse(&gps, GPS_lastNMEA(&gps)))
		{
			if (gps.latitudeDegrees != 0)
			{
				lato = gps.latitudeDegrees;
			}
			if (gps.longitudeDegrees != 0)
			{
				lono = gps.longitudeDegrees;
			}
			return;
		}
	}

}
static void gps_printData(void)
{
	// read data from the GPS in the 'main loop'
	char c = GPS_read(&gps);
	// if you want to debug, this is a good time to do it!
	char ca[1] = {c};
	if (GPSECHO && c) print(ca);
	// if a sentence is received, we can check the checksum, parse it...
	if (GPS_newNMEAreceived(&gps))
	{
		// a tricky thing here is if we print the NMEA sentence, or data
		// we end up not listening and catching other sentences!
		// so be very wary if using OUTPUT_ALLDATA and trying to print out data
		//println(GPS_lastNMEA(&gps)); // this also sets the newNMEAreceived() flag to false
		if (!GPS_parse(&gps, GPS_lastNMEA(&gps))) // this also sets the newNMEAreceived() flag to false
		  return; // we can fail to parse a sentence in which case we should just wait for another
	}
	// if millis() or timenow wraps around, we'll just reset it
	if (timenow > millis()) timenow = millis();

	// approximately every 2 seconds or so, print out the current stats
	if (millis() - timenow > 200)
	{
		timenow = millis(); // reset the timenow
		print("\r\nTime: ");
		if (gps.gpsTime.hour < 10) { print_char('0'); }
		print_int(gps.gpsTime.hour); print_char(':');
		if (gps.gpsTime.minute < 10) { print_char('0'); }
		print_int(gps.gpsTime.minute); print_char(':');
		if (gps.gpsTime.second < 10) { print_char('0'); }
		print_int(gps.gpsTime.second); print_char('.');
		if (gps.gpsTime.msecond < 10)
		{
			print("00");
		}
		else if (gps.gpsTime.msecond > 9 && gps.gpsTime.msecond < 100)
		{
			print("0");
		}
		print_int(gps.gpsTime.msecond);
		print("\r\nDate: ");
		print_int(gps.gpsTime.dayM); print_char('/');
		print_int(gps.gpsTime.month); print("/20");
		print_int(gps.gpsTime.year); println("");
		print("\r\nFix: "); print_int((int)gps.fix);
		print(" quality: "); print_int((int)gps.fixquality); println("");
		if (gps.fix)
		{
			print("Location: ");
			print_float(gps.latitudeDegrees); print_char(gps.lat);
			if (gps.latitudeDegrees != 0)
			{
				lato = gps.latitudeDegrees;
			}
			print(", ");
			print_float(gps.longitudeDegrees); print_char(gps.lon);
			if (gps.longitudeDegrees != 0)
			{
				lono = gps.longitudeDegrees;
			}
			print("Speed (knots): "); print_float(gps.speed);
			print("\r\nAngle: "); print_float(gps.angle);
			print("\r\nAltitude: "); print_float(gps.altitude);
			print("\r\nSatellites: "); print_int((int)gps.satellites);
		}
	}
}

static void bmp_getData(void)
{
	/*
	while (!bmp280_read_float(&bmp280, &temperature, &pressure, &humidity))
	{
			size = sprintf((char *)Data,
					"Temperature/pressure reading failed\n\r");
			CDC_Transmit_FS(Data, size);
			HAL_Delay(2000);
	}

	size = sprintf((char *)Data,"Pressure: %.2f Pa, Temperature: %.2f C \n\r",
			pressure, temperature);
	CDC_Transmit_FS(Data, size);
	if (bme280p) {
		size = sprintf((char *)Data,", Humidity: %.2f\n", humidity);
		CDC_Transmit_FS(Data, size);
	}

	else {
		size = sprintf((char *)Data, "\n\r");
		CDC_Transmit_FS(Data, size);
	}
	*/
	bmp280_read_float(&bmp280, &temperature, &pressure);
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


static bool bmp280_begin(void)
{
	bmp280.params = bmp280_default_config;
	bmp280.addr = BMP280_I2C_ADDRESS_0;
	bmp280.i2c = Get_I2C1_Instance();

	while (!bmp280_init(&bmp280, &bmp280.params)) {
		print("BMP280 initialization failed\n");

		//HAL_GPIO_TogglePin(LEDC_GPIO_Port, LEDC_Pin);
		HAL_Delay(500);
		return false;
	}

	print("BMP280 found!\n");
	return true;
}
static bool radio_begin(void)
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
		if (attempts >= 1000)
		{
			println("[LoRa] Too many attempts, aborting...");
			return false;
		}
	}

	radio.txDone = true;
	radio.rxDone = true;
	radio.newPacket = false;
	newIRQ = false;

	return true;
}
static bool sd_begin(void)
{
	//HAL_Delay(1000);
	println("[SD] Joining the server...");
	//HAL_Delay(1000);
	if (SD_init() == FR_OK)
	{
		println("[SD] joined the server.");
		print("[SD] Creating test file...");
		if (SD_newFile("/TEST.TXT") == FR_OK)
		{
			println("successful.");
			bool status = true;
			status |= (SD_writeToFile("text.txt", "Created on: ") == FR_OK);

			DateTime fileCreated = getTime();
			char dateStr[26] = {0};
			sprintf(dateStr, "%d-%d-20%d %d:%d:%d:%d", fileCreated.dayM, fileCreated.month, fileCreated.year,
													   fileCreated.hour, fileCreated.minute, fileCreated.second, fileCreated.msecond);

			status |= (SD_writeToFile("text.txt", dateStr) == FR_OK);

			if (status) println("[SD] Content writing successful!");
			else println("[SD] Content writing unsuccessful!");
		}
		else println("unsuccessful :(");
	}
	else
	{
		println("Player [SD] could not join the server!");
		return false;
	}

	return true;
}
static bool gps_begin(void)
{
	println("[GPS] Begin...");
	gps.uart = Get_UART3_Instance();
	GPS_init(&gps);
	GPS_sendCommand(&gps, PMTK_SET_NMEA_OUTPUT_ALLDATA);
	GPS_sendCommand(&gps, PMTK_SET_NMEA_UPDATE_5HZ);

	HAL_Delay(1000);

	// Ask for firmware version
	GPS_sendCommand(&gps, PMTK_Q_RELEASE);
	// TO ADD: chceck firmware response

	return true;
}


static void timingDebug(void)
{
	uint32_t start = 0;
	uint32_t stop = 0;

	start = millis();
	HAL_Delay(1000);
	stop = millis();

	printLen = sprintf(printBuffer, "Time: %lu ms\r\n", start);
	printv(printBuffer, printLen);
	printLen = sprintf(printBuffer, "Time: %lu ms\r\n", stop);
	printv(printBuffer, printLen);
	printLen = sprintf(printBuffer, "Space: %lu ms\r\n", stop - start);
	printv(printBuffer, printLen);


	start = micros();
	HAL_Delay(1000);
	stop = micros();

	printLen = sprintf(printBuffer, "Time: %lu us\r\n", start);
	printv(printBuffer, printLen);
	printLen = sprintf(printBuffer, "Time: %lu us\r\n", stop);
	printv(printBuffer, printLen);
	printLen = sprintf(printBuffer, "Space: %lu us\r\n", stop - start);
	printv(printBuffer, printLen);
}



static void algoGalgo(float yaw)
{
	float maxPower = 0.8;
	// Center: 180

	setMotors(yaw * maxPower * (1.0 / 360.0), (360.0 - yaw) * maxPower * (1.0 / 360.0));
}
