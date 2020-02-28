// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//		All sensor code
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#include "run.h"
#include "config.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

#include "gps.h"
#include "bmp280.h"
#include "logger.h"
#include "mpu9250.h"

#include "Peripherials/imuTest.c"

#define SENSING_DEBUG 1
#define SENSING_PRINT_DATA 1

GPS gps;
BMP280 bmp;
uint32_t lastDataPrint;
uint32_t lastBmpLog;
uint32_t lastImuLog;
uint32_t lastBmpRead;
// IMU
// (SPS)

static bool sensing_begin(void)
{
	uint8_t attempts = 0;

	attempts = 0;
	bmp.params = bmp280_default_config;
	bmp.addr = BMP280_I2C_ADDRESS_0;
	bmp.i2c = Get_I2C1_Instance();
	//unneccessary, to change
	while (!bmp280_init(&bmp, &bmp.params))
	{
		HAL_Delay(500);
		if (SENSING_DEBUG) println("[BMP] Init unsuccesfull, retrying...");
		attempts++;

		if (attempts >= 5)
		{
			if (SENSING_DEBUG) println("[BMP] Too many attempts, BMP is not active!");
			//log_print("Unable to init BMP");
			break;
		}
	}

	imuTest_begin();
	//if (imuTest_begin()) { println("[IMU] Init successful!"); log_print("IMU init success"); }
	//	else { log_print("Unable to init IMU"); }


	attempts = 0;
	gps.uart = Get_UART3_Instance();
	while (!GPS_init(&gps))
	{
		HAL_Delay(500);
		if (SENSING_DEBUG) println("[GPS] Init unsuccessful, retrying...");
		attempts++;

		if (attempts >= 5)
		{
			if (SENSING_DEBUG) println("[GPS] Too many attempts, GPS is not active!");
			//log_print("Unable to init GPS");
			break;
		}
	}

	if (SENSING_DEBUG)
	{
		println("Sensor init summary:");
		print("GPS - "); gps.active ? println("active") : println("not active");
		print("BMP - "); bmp.active ? println("active") : println("not active");
		print("IMU - "); imuActive ? println("active") : println("not active");
	}
}

static void sensing_loop(void)
{

	if (gps.active)
	{
		GPS_read(&gps);
		if (GPS_newNMEAreceived(&gps))
		{
			//println(GPS_lastNMEA(&gps));
			if (gps.fix) log_gps(&gps);
			GPS_parse(&gps, GPS_lastNMEA(&gps));
		}
	}

	if (bmp.active && millis() - lastBmpRead >= 50)
	{

		//bmp280_read_float(&bmp, tee, pee);
		bmp280_update(&bmp);
		lastBmpRead = millis();
		if (millis() - lastBmpLog >= 100)
		{
			log_bmp(&bmp);
			lastBmpLog = millis();
		}
	}


	if (imuActive)
	{
		imuTest_getData();		// get data from IMU
		imuTest_quatUpdate();	// compute data received

		if (millis() - lastImuLog >= 100)
		{
			imuTest_getEuler();
			float eulers[3] = {yaw, pitch, roll};
			log_imu(eulers);
			lastImuLog = millis();
		}

	}


	if (millis() - lastDataPrint >= 1000 && SENSING_PRINT_DATA)
	{
		if (bmp.active)
		{
			print("Pressure: "); print_float(bmp.pressure); println("");
			print("Temperature: "); print_float(bmp.temperature); println("");
		}
		if (gps.active)
		{
			if (gps.fix)
			{
				HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_SET);
				print("Latitude: "); print_float(gps.latitudeDegrees); println("");
				print("Longitude: "); print_float(gps.longitudeDegrees); println("");
			}
			else
			{
				HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_RESET);
				println("GPS has no fix!");
			}
		}
		lastDataPrint = millis();
	}
}
