#ifndef SENSING_C
#define SENSING_C

#include "run.h"
#include "config.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdbool.h>

#include "gps.h"
#include "bmp280.h"
#include "mpu9250.h"
#include "sps30.h"

uint32_t lastBmpRead;
uint32_t lastSpsRead;

static bool sensing_setup(void)
{
	(*Common.log_print)("*S00"); // [SENSING] Hello!
	uint8_t attempts;
	#if BMP_ENABLE
		attempts = 0;
		Common.bmp.i2c_addr = BMP280_I2C_ADDRESS;
		Common.bmp.i2c = Get_I2C1_Instance();
		#if SENSING_DEBUG
			println("[SENSING] Initializing BMP280");
		#endif
		(*Common.log_print)("*S10");
		
		while (!bmp280_init(&(Common.bmp), &bmp280_default_config))
		{
			if (++attempts > 5)
			{
				#if SENSING_DEBUG
					println("error: [SENSING] No connection with BMP280, sensor is not active");
				#endif
				(*Common.log_print)("*ES10");
				break;
			}

			delay(500);
			#if SENSING_DEBUG
				println("[SENSING] BMP280 init unsuccesfull, retrying...");
			#endif
		}
	#else
		#if SENSING_DEBUG
			println("warning: [SENSING] BMP280 DISABLED!");
		#endif
		(*Common.log_print)("*WS10");
		Common.bmp.active = false;
	#endif

	#if GPS_ENABLE
		attempts = 0;
		Common.gps.uart = Get_UART1_Instance();
		#if SENSING_DEBUG
			println("[SENSING] Initializing L86");
		#endif
		(*Common.log_print)("*S20");

		while (!GPS_init(&(Common.gps)))
		{
			if (++attempts > 5)
			{
				#if SENSING_DEBUG
					println("error: [SENSING] No connection with L86, no GPS data will be avaliable");
				#endif
				(*Common.log_print)("*ES20");
				break;
			}

			delay(500);
			#if SENSING_DEBUG
				println("[SENSING] L86 init unsuccesfull, retrying...");
			#endif
		}
		Common.gps.active = true; //######################
		Common.gps.paused = false;
		if (Common.gps.active && !Common.gps.fix)
		{
			Common.gps.latitudeDegrees = DEFAULT_TARGET_LAT;
			Common.gps.longitudeDegrees = DEFAULT_TARGET_LON;
			Common.gps.altitude = DEFAULT_TARGET_ALT;
		}
	#else
		#if SENSING_DEBUG
			println("warning: [SENSING] L86 DISABLED!");
		#endif
		(*Common.log_print)("*WS20");
		Common.gps.active = false;
	#endif

	#if IMU_ENABLE
		attempts = 0;
		Common.mpu.i2c_addr = MPU9250_I2C_ADDRESS;
		Common.mpu.i2c_addr_ak = AK8963_I2C_ADDRESS;
		Common.mpu.i2c = Get_I2C1_Instance();
		#if SENSING_DEBUG
			println("[SENSING] Initializing MPU9250");
		#endif
		(*Common.log_print)("*S30");

		Common.mpu.active = true;
		while (!MPU9250_present(&(Common.mpu)))
		{
			if (++attempts > 5)
			{
				#if SENSING_DEBUG
					println("error: [SENSING] No connection with MPU9250, no IMU data will be avaliable");
				#endif
				(*Common.log_print)("*ES30");
				Common.mpu.active = false;
				break;
			}

			delay(500);
			#if SENSING_DEBUG
				println("[SENSING] MPU9250 init unsuccesfull, retrying...");
			#endif
		}
		if (Common.mpu.active)
		{
			#if SENSING_DEBUG
				println("[SENSING] Initializing AK8963");
			#endif
			(*Common.log_print)("*S31");
			while (!AK8963_present(&(Common.mpu)) && false) //####################
			{
				if (++attempts > 5)
				{
					#if SENSING_DEBUG
						println("error: [SENSING] No connection with AK8963, no IMU data will be avaliable");
					#endif
					(*Common.log_print)("*ES31");
					Common.mpu.active = false;
					break;
				}

				delay(500);
				#if SENSING_DEBUG
					println("[SENSING] AK8963 init unsuccesfull, retrying...");
				#endif
			}
		}
		if (Common.mpu.active)
		{	
			#if SENSING_DEBUG
					println("[SENSING] Checking MPU9250 readings. Don't move the board!");
			#endif
			(*Common.log_print)("*S32");
			attempts = 0;
			writePin(LEDA, HIGH);
			delay(500);
			while (!MPU9250_SelfTest(&(Common.mpu)))
			{
				if (++attempts > 5)
				{
					#if SENSING_DEBUG
						println("warning: [SENSING] MPU9250 SelfTest not passed, performance issues may occur");
					#endif
					(*Common.log_print)("*WS32");
					break;
				}
				
				delay(500);
				#if SENSING_DEBUG
					println("[SENSING] MPU9250 SelfTest failed, retrying...");
				#endif
			}
			(*Common.log_print)("*S33");
			if (mpu9250_default_config.calibrate)
			{
				#if SENSING_DEBUG
					println("[SENSING] Performing MPU9250 calibration. Don't move the board!");
				#endif
			}
			else
			{
				#if SENSING_DEBUG
					println("warning: [SENSING] Skipping IMU calibration!");
				#endif
				(*Common.log_print)("*WS33");
			}
			MPU9250_init(&(Common.mpu), &mpu9250_default_config);
			writePin(LEDA, LOW);
			writePin(LEDB, HIGH);
			if (mpu9250_default_config.calibrate)
			{
				#if SENSING_DEBUG
					println("[SENSING] Performing AK8963 calibration. Rotate the board.");
				#endif
			}
			AK8963_init(&(Common.mpu), &mpu9250_default_config);
			writePin(LEDB, LOW);
		}
	#else
		#if SENSING_DEBUG
			println("warning: [SENSING] MPU9250 DISABLED!");
		#endif
		(*Common.log_print)("*WS30");
		Common.mpu.active = false;
	#endif

	#if SPS_ENABLE
		attempts = 0;
		Common.sps.uart = Get_UART2_Instance();
		#if SENSING_DEBUG
			println("[SENSING] Initializing SPS30");
		#endif
		(*Common.log_print)("*S40");

		while (!SPS30_init(&(Common.sps)))
		{
			if (++attempts > 5)
			{
				#if SENSING_DEBUG
					println("error: [SENSING] No connection with SPS30, sensor is not active");
				#endif
				(*Common.log_print)("*ES40");
				break;
			}

			delay(500);
			#if SENSING_DEBUG
				println("[SENSING] SPS30 init unsuccesfull, retrying...");
			#endif
		}
	#else
		#if SENSING_DEBUG
			println("warning: [SENSING] SPS30 DISABLED!");
		#endif
		(*Common.log_print)("*WS40");
		Common.sps.active = false;
	#endif

	#if SENSING_DEBUG
		println("[SENSING] Sensor init summary:");
		println("- BMP %s", Common.bmp.active ? "active" : "not active");
		println("- GPS %s", Common.gps.active ? "active" : "not active");
		println("- IMU %s", Common.mpu.active ? "active" : "not active");
		println("- SPS %s", Common.sps.active ? "active" : "not active");
	#endif
	if (!Common.bmp.active || !Common.gps.active || !Common.mpu.active || !Common.sps.active) (*Common.log_print)("*WS00");

	return (Common.bmp.active || !BMP_ENABLE) && (Common.gps.active || !GPS_ENABLE) && (Common.mpu.active || !IMU_ENABLE) && (Common.sps.active || !SPS_ENABLE);
}

static void sensing_loop(void)
{
	#if BMP_ENABLE
		if (Common.bmp.active && millis() - lastBmpRead >= SENSING_BMP_DELAY)
		{
			Common.bmp.alt_dx = Common.bmp.altitude;
			bmp280_update(&(Common.bmp));
			Common.bmp.alt_dx -= Common.bmp.altitude;
			Common.bmp.alt_dx *= 1000 / (millis() - lastBmpRead);
			lastBmpRead = millis();
		}
	#endif

	#if GPS_ENABLE
		if (Common.gps.active)
		{
			GPS_update(&(Common.gps));
			//writePin(LEDC, Common.gps.fix);
		}
	#endif

	#if IMU_ENABLE
		if (Common.mpu.active) MPU9250_update(&(Common.mpu));
	#endif

	#if SPS_ENABLE
		if (Common.sps.active && millis() - lastSpsRead >= SENSING_SPS_DELAY) { SPS30_update(&(Common.sps)); lastSpsRead = millis(); }
	#endif

	//SPS30
}

#endif
