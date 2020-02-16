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

SX1278 radio;
uint8_t sendBuffer[SX1278_MAX_PACKET];
uint8_t message_length;

GPS gps;

bool newIRQ;
uint32_t timenow;

float SelfTest[6];
float MPU9250gyroBias[3];
float MPU9250accelBias[3];
float MPU9250magBias[3];      // Bias corrections for gyro and accelerometer
float sum;
uint32_t sumCount;
uint32_t lastPrint;

// Servo : TIM3->CCR3 (500;1000)

static void setup(void)
{
	setupPins();

	//while (HAL_GPIO_ReadPin(BTN_USR_GPIO_Port, BTN_USR_Pin) == GPIO_PIN_SET);

	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_SET);
	println("Hello world!!");	HAL_Delay(500);
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_RESET);

	//if (bmp280_begin()) println("[BMP] joined the server!");
	//if (sd_begin()) println("[SD] joined the server!");
	//if (radio_begin()) println("[LoRa] joined the server!");
	//enableMotors(); println("[MOT] joined the server!");
	if (mpu_begin()) println("[MPU] joined the server!");
	//if (gps_begin()) println("[GPS] joined the server!");

	//ser1.ccr = &(TIM3->CCR3);
}

static void loop(void)
{
	mpu_printData();
	//if (sd_begin()) println("[SD] joined the server!");
	//gps_printData();
	//radio_receive();
	//radio_transmit();
	//mot_up_down();
}

static void mot_up_down(void)
{
	uint8_t i;
	for (i = 0; i < 200; i++)
	{
		if (HAL_GPIO_ReadPin(BTN_USR_GPIO_Port, BTN_USR_Pin) == GPIO_PIN_RESET)
		{
			setMotors((float)i / 255, (float)i / 255);
			HAL_GPIO_WritePin(LEDD_GPIO_Port, LEDD_Pin, GPIO_PIN_SET);
		}
		else
		{
			haltMotors();
			HAL_GPIO_WritePin(LEDD_GPIO_Port, LEDD_Pin, GPIO_PIN_RESET);
		}
	}
	for (i = 200; i > 0; i--)
	{
		if (HAL_GPIO_ReadPin(BTN_USR_GPIO_Port, BTN_USR_Pin) == GPIO_PIN_RESET)
		{
			setMotors((float)i / 255, (float)i / 255);
			HAL_GPIO_WritePin(LEDD_GPIO_Port, LEDD_Pin, GPIO_PIN_SET);
		}
		else
		{
			haltMotors();
			HAL_GPIO_WritePin(LEDD_GPIO_Port, LEDD_Pin, GPIO_PIN_RESET);
		}

		HAL_Delay(100);
	}
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
			print(", ");
			print_float(gps.longitudeDegrees); print_char(gps.lon);
			print("Speed (knots): "); print_float(gps.speed);
			print("\r\nAngle: "); print_float(gps.angle);
			print("\r\nAltitude: "); print_float(gps.altitude);
			print("\r\nSatellites: "); print_int((int)gps.satellites);
		}
	}
}
static void mpu_printData(void)
{
	if (MPU_readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
	{  // check if data ready interrupt
		MPU_readAccelData(accelCount);  // Read the x/y/z adc values

		// Now we'll calculate the accleration value into actual g's
		ax = (float)accelCount[0]*aRes - MPU9250accelBias[0];  // get actual g value, this depends on scale being set
		ay = (float)accelCount[1]*aRes - MPU9250accelBias[1];
		az = (float)accelCount[2]*aRes - MPU9250accelBias[2];

		MPU_readGyroData(gyroCount);  // Read the x/y/z adc values

		// Calculate the gyro value into actual degrees per second
		gx = (float)gyroCount[0]*gRes;  // get actual gyro value, this depends on scale being set
		gy = (float)gyroCount[1]*gRes;
		gz = (float)gyroCount[2]*gRes;

		MPU_readMagData(magCount);  // Read the x/y/z adc values

		// Calculate the magnetometer values in milliGauss
		// Include factory calibration per data sheet and user environmental corrections
		mx = (float)magCount[0]*mRes*magCalibration[0] - MPU9250magBias[0];  // get actual magnetometer value, this depends on scale being set
		my = (float)magCount[1]*mRes*magCalibration[1] - MPU9250magBias[1];
		mz = (float)magCount[2]*mRes*magCalibration[2] - MPU9250magBias[2];
	}

	Now = micros();
	deltat = ((Now - lastUpdate)/1000000.0f); // set integration time by time elapsed since last filter update
	lastUpdate = Now;

	sum += deltat; // sum for averaging filter update rate
	sumCount++;

	// Sensors x (y)-axis of the accelerometer/gyro is aligned with the y (x)-axis of the magnetometer;
	// the magnetometer z-axis (+ down) is misaligned with z-axis (+ up) of accelerometer and gyro!
	// We have to make some allowance for this orientation mismatch in feeding the output to the quaternion filter.
	// For the MPU9250+MS5637 Mini breakout the +x accel/gyro is North, then -y accel/gyro is East. So if we want te quaternions properly aligned
	// we need to feed into the Madgwick function Ax, -Ay, -Az, Gx, -Gy, -Gz, My, -Mx, and Mz. But because gravity is by convention
	// positive down, we need to invert the accel data, so we pass -Ax, Ay, Az, Gx, -Gy, -Gz, My, -Mx, and Mz into the Madgwick
	// function to get North along the accel +x-axis, East along the accel -y-axis, and Down along the accel -z-axis.
	// This orientation choice can be modified to allow any convenient (non-NED) orientation convention.
	// Pass gyro rate as rad/s
	MadgwickQuaternionUpdate(-ax, ay, az, gx*PI/180.0f, -gy*PI/180.0f, -gz*PI/180.0f,  my,  -mx, mz);
	//  MahonyQuaternionUpdate(-ax, ay, az, gx*pi/180.0f, -gy*pi/180.0f, -gz*pi/180.0f,  my,  -mx, mz);

	// Serial print and/or display at 0.5 s rate independent of data rates
	//delt_t = millis() - count;
	//if (delt_t > 1000)
	if (millis() - lastPrint > 10)
	{ // update LCD once per half-second independent of read rate
		/*
		println("MPU9250: ");
		print("ax = "); print_int((int)1000*ax);
		print(" ay = "); print_int((int)1000*ay);
		print(" az = "); print_int((int)1000*az); println(" mg");
		print("gx = "); print_float(gx);
		print(" gy = "); print_float(gy);
		print(" gz = "); print_float(gz); println(" deg/s");
		print("mx = "); print_int((int)mx);
		print(" my = "); print_int((int)my);
		print(" mz = "); print_int((int)mz); println(" mG");
		 */
		/*
		print("q0 = "); print_float(q[0]);
		print(" qx = "); print_float(q[1]);
		print(" qy = "); print_float(q[2]);
		print(" qz = "); print_float(q[3]); println("");
		*/
		tempCount = MPU_readTempData();  // Read the gyro adc values
		temperature = ((float) tempCount) / 333.87 + 21.0; // Gyro chip temperature in degrees Centigrade
		// Print temperature in degrees Centigrade


		yaw   = atan2f(2.0f * (q[1] * q[2] + q[0] * q[3]), q[0] * q[0] + q[1] * q[1] - q[2] * q[2] - q[3] * q[3]);
		pitch = -asinf(2.0f * (q[1] * q[3] - q[0] * q[2]));
		roll  = atan2f(2.0f * (q[0] * q[1] + q[2] * q[3]), q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3]);
		pitch *= 180.0f / PI;
		yaw   *= 180.0f / PI;
		//yaw   += 13.8f; // Declination at Danville, California is 13 degrees 48 minutes and 47 seconds on 2014-04-04
		if(yaw < 0) yaw   += 360.0f; // Ensure yaw stays between 0 and 360
		roll  *= 180.0f / PI;

		//print("Yaw: "); print_float(yaw); print(" , Pitch: "); print_float(pitch); print(" , Roll: "); print_float(roll); println("");
		print_int(-360); print(" ");
		print_float(yaw); print(" "); print_float(pitch); print(" "); print_float(roll); print(" ");
		print_int(360); println("");

		//print("MPU9250 Gyro temperature is ");  print_float(temperature);  println(" degrees C"); // Print T values to tenths of s degree C
		lastPrint = millis();
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


static bool bmp280_begin(void)
{
	bmp280.params = bmp280_default_config;
	bmp280.addr = BMP280_I2C_ADDRESS_0;
	bmp280.i2c = Get_I2C1_Instance();

	while (!bmp280_init(&bmp280, &bmp280.params)) {
		print("BMP280 initialization failed\n");

		HAL_GPIO_TogglePin(LEDC_GPIO_Port, LEDC_Pin);
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

	//radio.useDio0IRQ = true; println("[LoRa] I am using DIO0 interrupt.");
	radio.useDio0IRQ = false; println("[LoRa] I am not using DIO0 interrupt.");

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
	HAL_Delay(1000);
	println("[SD] Joining the server...");
	HAL_Delay(1000);
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


static void setupPins(void)
{
	HAL_GPIO_WritePin(LEDA_GPIO_Port, LEDA_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LEDC_GPIO_Port, LEDC_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LEDD_GPIO_Port, LEDD_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PH_L_GPIO_Port, PH_L_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PH_R_GPIO_Port, PH_R_Pin, GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(EN_L_GPIO_Port, EN_L_Pin, GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(EN_R_GPIO_Port, EN_R_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(P1_GPIO_Port, P1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(P2_GPIO_Port, P2_Pin, GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(P3_GPIO_Port, P3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(P4_GPIO_Port, P4_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(P5_GPIO_Port, P5_Pin, GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(P6_GPIO_Port, P6_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(P7_GPIO_Port, P7_Pin, GPIO_PIN_RESET);
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

static bool mpu_begin(void)
{
	if (MPU_present())
	{
		MPU_SelfTest(SelfTest); // Start by performing self test and reporting values
		println("MPU9250 Self Test:");
		print("x-axis self test: acceleration trim within : "); print_float(SelfTest[0]); println("% of factory value");
		print("y-axis self test: acceleration trim within : "); print_float(SelfTest[1]); println("% of factory value");
		print("z-axis self test: acceleration trim within : "); print_float(SelfTest[2]); println("% of factory value");
		print("x-axis self test: gyration trim within : "); print_float(SelfTest[3]); println("% of factory value");
		print("y-axis self test: gyration trim within : "); print_float(SelfTest[4]); println("% of factory value");
		print("z-axis self test: gyration trim within : "); print_float(SelfTest[5]); println("% of factory value");
		HAL_Delay(1000);

		// get sensor resolutions, only need to do this once
		MPU_getAres();
		MPU_getGres();
		MPU_getMres();

		println(" Calibrate MPU9250 gyro and accel");
		MPU_calibrate(MPU9250gyroBias, MPU9250accelBias); // Calibrate gyro and accelerometers, load biases in bias registers
		println("accel biases (mg)");
		print_float(1000.0 * MPU9250accelBias[0]); println("");
		print_float(1000.0 * MPU9250accelBias[1]); println("");
		print_float(1000.0 * MPU9250accelBias[2]); println("");
		println("gyro biases (dps)");
		print_float(MPU9250gyroBias[0]); println("");
		print_float(MPU9250gyroBias[1]); println("");
		print_float(MPU9250gyroBias[2]); println("");

		HAL_Delay(1000);

		MPU_init();
		println("MPU9250 initialized for active data mode...."); // Initialize device for active mode read of acclerometer, gyroscope, and temperature

		// Read the WHO_AM_I register of the magnetometer, this is a good test of communication
		char d = MPU_readByte(AK8963_ADDRESS, AK8963_WHO_AM_I);  // Read WHO_AM_I register for AK8963
		if (d == 0x48) println("[MAG] SUCCESSS!!!!");
		HAL_Delay(1000);

		// Get magnetometer calibration from AK8963 ROM
		AK8963_init(magCalibration); println("AK8963 initialized for active data mode...."); // Initialize device for active mode read of magnetometer

		MPU_calibrateMag(MPU9250magBias);
		println("AK8963 mag biases (mG)");
		print_float(MPU9250magBias[0]); println("");
		print_float(MPU9250magBias[1]); println("");
		print_float(MPU9250magBias[2]); println("");
		HAL_Delay(2000); // add delay to see results before serial spew of data

		print("[MAG] X-Axis sensitivity adjustment value "); print_float(magCalibration[0]); println("");
		print("[MAG] Y-Axis sensitivity adjustment value "); print_float(magCalibration[1]); println("");
		print("[MAG] Z-Axis sensitivity adjustment value "); print_float(magCalibration[2]); println("");
	}
}
