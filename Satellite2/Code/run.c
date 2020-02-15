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


typedef struct
{
	float angle;
	uint32_t* ccr;

} Servo;

static void setServo(Servo inst, float angle)
{
	inst.angle = angle;
	// CCR1 = 500 -> 1ms -> 0deg
	// CCR1 = 1000 -> 2ms -> 180deg
	*(inst.ccr) = 500 * (uint32_t)(angle / 180.0) + 500;
}


BMP280 bmp280;

SX1278 radio;
uint8_t sendBuffer[SX1278_MAX_PACKET];
uint8_t message_length;

GPS gps;

bool newIRQ;
uint32_t timenow;

Servo ser1;

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
	//if (mpu_begin()) println("[MPU] joined the server!");

	printLen = sprintf(printBuffer, "Time: %lu ms\r\n", millis());
	printv(printBuffer, printLen);
	HAL_Delay(100);
	printLen = sprintf(printBuffer, "Time: %lu ms\r\n", millis());
	printv(printBuffer, printLen);

	gps_begin();

	//ser1.ccr = &(TIM3->CCR3);
}

static void loop(void)
{
	//if (sd_begin()) println("[SD] joined the server!");
	//HAL_Delay(1000);
	//mpu_printData();
	gps_printData();
	//gps_begin();
	//radio_receive();
	//radio_transmit();
	//setMotors(254, 254);
	//mot_up_down();
	//HAL_Delay(1000);

	/*
	uint32_t a = 0;
	for (a = 0; a <= 500; a += 1)
	{
		//setServo(ser1, a);
		TIM3->CCR3 = a + 500;
		HAL_Delay(10);
	}
	*/
/*
	TIM3->CCR3 = 500;
	println("500");
	HAL_Delay(1000);
	TIM3->CCR3 = 1000;
	println("1000");
	HAL_Delay(1000);
*/
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

		timenow = millis();
		printLen = sprintf(printBuffer, "%ul ms -->", timenow);
		printv(printBuffer, printLen);
		HAL_Delay(100);

		timenow = millis();
		printLen = sprintf(printBuffer, "%ul ms\r\n", timenow);
		printv(printBuffer, printLen);
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
	char* ca[1] = {c};
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
	if (millis() - timenow > 2000)
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
	// 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
	GPS_init(&gps);
	// uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
	GPS_sendCommand(&gps, PMTK_SET_NMEA_OUTPUT_ALLDATA);
	// uncomment this line to turn on only the "minimum recommended" data
	//GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
	// For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
	// the parser doesn't care about other sentences at this time
	// Set the update rate
	GPS_sendCommand(&gps, PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
	// For the parsing code to work nicely and have time to sort thru the data, and
	// print it out we don't suggest using anything higher than 1 Hz

	// Request updates on antenna status, comment out to keep quiet
	GPS_sendCommand(&gps, PGCMD_ANTENNA);

	HAL_Delay(1000);

	// Ask for firmware version
	GPS_sendCommand(&gps, PMTK_Q_RELEASE);
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

static void accelgyrocalMPU9250(float * dest1, float * dest2)
{
  uint8_t data[12]; // data array to hold accelerometer and gyro x, y, z, data
  uint16_t ii, packet_count, fifo_count;
  int32_t gyro_bias[3]  = {0, 0, 0}, accel_bias[3] = {0, 0, 0};

 // reset device
  MPU_writeByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x80); // Write a one to bit 7 reset bit; toggle reset device
  HAL_Delay(100);

 // get stable time source; Auto select clock source to be PLL gyroscope reference if ready
 // else use the internal oscillator, bits 2:0 = 001
  MPU_writeByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x01);
  MPU_writeByte(MPU9250_ADDRESS, PWR_MGMT_2, 0x00);
  HAL_Delay(200);

// Configure device for bias calculation
  MPU_writeByte(MPU9250_ADDRESS, INT_ENABLE, 0x00);   // Disable all interrupts
  MPU_writeByte(MPU9250_ADDRESS, FIFO_EN, 0x00);      // Disable FIFO
  MPU_writeByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x00);   // Turn on internal clock source
  MPU_writeByte(MPU9250_ADDRESS, I2C_MST_CTRL, 0x00); // Disable I2C master
  MPU_writeByte(MPU9250_ADDRESS, USER_CTRL, 0x00);    // Disable FIFO and I2C master modes
  MPU_writeByte(MPU9250_ADDRESS, USER_CTRL, 0x0C);    // Reset FIFO and DMP
  HAL_Delay(15);

// Configure MPU6050 gyro and accelerometer for bias calculation
  MPU_writeByte(MPU9250_ADDRESS, CONFIG, 0x01);      // Set low-pass filter to 188 Hz
  MPU_writeByte(MPU9250_ADDRESS, SMPLRT_DIV, 0x00);  // Set sample rate to 1 kHz
  MPU_writeByte(MPU9250_ADDRESS, GYRO_CONFIG, 0x00);  // Set gyro full-scale to 250 degrees per second, maximum sensitivity
  MPU_writeByte(MPU9250_ADDRESS, ACCEL_CONFIG, 0x00); // Set accelerometer full-scale to 2 g, maximum sensitivity

  uint16_t  gyrosensitivity  = 131;   // = 131 LSB/degrees/sec
  uint16_t  accelsensitivity = 16384;  // = 16384 LSB/g

// Configure FIFO to capture accelerometer and gyro data for bias calculation
  MPU_writeByte(MPU9250_ADDRESS, USER_CTRL, 0x40);   // Enable FIFO
  MPU_writeByte(MPU9250_ADDRESS, FIFO_EN, 0x78);     // Enable gyro and accelerometer sensors for FIFO  (max size 512 bytes in MPU-9150)
  HAL_Delay(40); // accumulate 40 samples in 40 milliseconds = 480 bytes

// At end of sample accumulation, turn off FIFO sensor read
  MPU_writeByte(MPU9250_ADDRESS, FIFO_EN, 0x00);        // Disable gyro and accelerometer sensors for FIFO
  MPU_readBytes(MPU9250_ADDRESS, FIFO_COUNTH, 2, &data[0]); // read FIFO sample count
  fifo_count = ((uint16_t)data[0] << 8) | data[1];
  packet_count = fifo_count/12;// How many sets of full gyro and accelerometer data for averaging

  for (ii = 0; ii < packet_count; ii++) {
    int16_t accel_temp[3] = {0, 0, 0}, gyro_temp[3] = {0, 0, 0};
    MPU_readBytes(MPU9250_ADDRESS, FIFO_R_W, 12, &data[0]); // read data for averaging
    accel_temp[0] = (int16_t) (((int16_t)data[0] << 8) | data[1]  ) ;  // Form signed 16-bit integer for each sample in FIFO
    accel_temp[1] = (int16_t) (((int16_t)data[2] << 8) | data[3]  ) ;
    accel_temp[2] = (int16_t) (((int16_t)data[4] << 8) | data[5]  ) ;
    gyro_temp[0]  = (int16_t) (((int16_t)data[6] << 8) | data[7]  ) ;
    gyro_temp[1]  = (int16_t) (((int16_t)data[8] << 8) | data[9]  ) ;
    gyro_temp[2]  = (int16_t) (((int16_t)data[10] << 8) | data[11]) ;

    accel_bias[0] += (int32_t) accel_temp[0]; // Sum individual signed 16-bit biases to get accumulated signed 32-bit biases
    accel_bias[1] += (int32_t) accel_temp[1];
    accel_bias[2] += (int32_t) accel_temp[2];
    gyro_bias[0]  += (int32_t) gyro_temp[0];
    gyro_bias[1]  += (int32_t) gyro_temp[1];
    gyro_bias[2]  += (int32_t) gyro_temp[2];

}
    accel_bias[0] /= (int32_t) packet_count; // Normalize sums to get average count biases
    accel_bias[1] /= (int32_t) packet_count;
    accel_bias[2] /= (int32_t) packet_count;
    gyro_bias[0]  /= (int32_t) packet_count;
    gyro_bias[1]  /= (int32_t) packet_count;
    gyro_bias[2]  /= (int32_t) packet_count;

  if(accel_bias[2] > 0L) {accel_bias[2] -= (int32_t) accelsensitivity;}  // Remove gravity from the z-axis accelerometer bias calculation
  else {accel_bias[2] += (int32_t) accelsensitivity;}

// Construct the gyro biases for push to the hardware gyro bias registers, which are reset to zero upon device startup
  data[0] = (-gyro_bias[0]/4  >> 8) & 0xFF; // Divide by 4 to get 32.9 LSB per deg/s to conform to expected bias input format
  data[1] = (-gyro_bias[0]/4)       & 0xFF; // Biases are additive, so change sign on calculated average gyro biases
  data[2] = (-gyro_bias[1]/4  >> 8) & 0xFF;
  data[3] = (-gyro_bias[1]/4)       & 0xFF;
  data[4] = (-gyro_bias[2]/4  >> 8) & 0xFF;
  data[5] = (-gyro_bias[2]/4)       & 0xFF;

// Push gyro biases to hardware registers
  MPU_writeByte(MPU9250_ADDRESS, XG_OFFSET_H, data[0]);
  MPU_writeByte(MPU9250_ADDRESS, XG_OFFSET_L, data[1]);
  MPU_writeByte(MPU9250_ADDRESS, YG_OFFSET_H, data[2]);
  MPU_writeByte(MPU9250_ADDRESS, YG_OFFSET_L, data[3]);
  MPU_writeByte(MPU9250_ADDRESS, ZG_OFFSET_H, data[4]);
  MPU_writeByte(MPU9250_ADDRESS, ZG_OFFSET_L, data[5]);

// Output scaled gyro biases for display in the main program
  dest1[0] = (float) gyro_bias[0]/(float) gyrosensitivity;
  dest1[1] = (float) gyro_bias[1]/(float) gyrosensitivity;
  dest1[2] = (float) gyro_bias[2]/(float) gyrosensitivity;

// Construct the accelerometer biases for push to the hardware accelerometer bias registers. These registers contain
// factory trim values which must be added to the calculated accelerometer biases; on boot up these registers will hold
// non-zero values. In addition, bit 0 of the lower byte must be preserved since it is used for temperature
// compensation calculations. Accelerometer bias registers expect bias input as 2048 LSB per g, so that
// the accelerometer biases calculated above must be divided by 8.

  int32_t accel_bias_reg[3] = {0, 0, 0}; // A place to hold the factory accelerometer trim biases
  MPU_readBytes(MPU9250_ADDRESS, XA_OFFSET_H, 2, &data[0]); // Read factory accelerometer trim values
  accel_bias_reg[0] = (int32_t) (((int16_t)data[0] << 8) | data[1]);
  MPU_readBytes(MPU9250_ADDRESS, YA_OFFSET_H, 2, &data[0]);
  accel_bias_reg[1] = (int32_t) (((int16_t)data[0] << 8) | data[1]);
  MPU_readBytes(MPU9250_ADDRESS, ZA_OFFSET_H, 2, &data[0]);
  accel_bias_reg[2] = (int32_t) (((int16_t)data[0] << 8) | data[1]);

  uint32_t mask = 1uL; // Define mask for temperature compensation bit 0 of lower byte of accelerometer bias registers
  uint8_t mask_bit[3] = {0, 0, 0}; // Define array to hold mask bit for each accelerometer bias axis

  for(ii = 0; ii < 3; ii++) {
    if((accel_bias_reg[ii] & mask)) mask_bit[ii] = 0x01; // If temperature compensation bit is set, record that fact in mask_bit
  }

  // Construct total accelerometer bias, including calculated average accelerometer bias from above
  accel_bias_reg[0] -= (accel_bias[0]/8); // Subtract calculated averaged accelerometer bias scaled to 2048 LSB/g (16 g full scale)
  accel_bias_reg[1] -= (accel_bias[1]/8);
  accel_bias_reg[2] -= (accel_bias[2]/8);

  data[0] = (accel_bias_reg[0] >> 8) & 0xFF;
  data[1] = (accel_bias_reg[0])      & 0xFF;
  data[1] = data[1] | mask_bit[0]; // preserve temperature compensation bit when writing back to accelerometer bias registers
  data[2] = (accel_bias_reg[1] >> 8) & 0xFF;
  data[3] = (accel_bias_reg[1])      & 0xFF;
  data[3] = data[3] | mask_bit[1]; // preserve temperature compensation bit when writing back to accelerometer bias registers
  data[4] = (accel_bias_reg[2] >> 8) & 0xFF;
  data[5] = (accel_bias_reg[2])      & 0xFF;
  data[5] = data[5] | mask_bit[2]; // preserve temperature compensation bit when writing back to accelerometer bias registers

// Apparently this is not working for the acceleration biases in the MPU-9250
// Are we handling the temperature correction bit properly?
// Push accelerometer biases to hardware registers
/*  MPU_writeByte(MPU9250_ADDRESS, XA_OFFSET_H, data[0]);
  MPU_writeByte(MPU9250_ADDRESS, XA_OFFSET_L, data[1]);
  MPU_writeByte(MPU9250_ADDRESS, YA_OFFSET_H, data[2]);
  MPU_writeByte(MPU9250_ADDRESS, YA_OFFSET_L, data[3]);
  MPU_writeByte(MPU9250_ADDRESS, ZA_OFFSET_H, data[4]);
  MPU_writeByte(MPU9250_ADDRESS, ZA_OFFSET_L, data[5]);
*/
// Output scaled accelerometer biases for display in the main program
   dest2[0] = (float)accel_bias[0]/(float)accelsensitivity;
   dest2[1] = (float)accel_bias[1]/(float)accelsensitivity;
   dest2[2] = (float)accel_bias[2]/(float)accelsensitivity;
}


static bool mpu_begin(void)
{
	HAL_Delay(1000);
	char c = MPU_readByte(MPU9250_ADDRESS, MPU9250_WHO_AM_I);  // Read WHO_AM_I register for MPU-9250
	print_int((int)c);
	println(" <- should be 0x71");


	float SelfTest[6];
	float MPU9250gyroBias[3] = {0, 0, 0};
	float MPU9250accelBias[3] = {0, 0, 0};
	float MPU9250magBias[3] = {0, 0, 0};      // Bias corrections for gyro and accelerometer

	if (c == 0x71)
	{
		println("MPU9250 is online...");

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
		accelgyrocalMPU9250(MPU9250gyroBias, MPU9250accelBias); // Calibrate gyro and accelerometers, load biases in bias registers
		println("accel biases (mg)");
		print_float(1000.*MPU9250accelBias[0]); println("");
		print_float(1000.*MPU9250accelBias[1]); println("");
		print_float(1000.*MPU9250accelBias[2]); println("");
		println("gyro biases (dps)");
		print_float(MPU9250gyroBias[0]); println("");
		print_float(MPU9250gyroBias[1]); println("");
		print_float(MPU9250gyroBias[2]); println("");

		HAL_Delay(1000);

		MPU_init();
		println("MPU9250 initialized for active data mode...."); // Initialize device for active mode read of acclerometer, gyroscope, and temperature
	}
}
