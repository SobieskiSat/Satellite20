// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//		A code to test MPU9250
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#include "run.h"
#include "main.h"
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

#include "mpu9250.h"
#include "clock.h"
#include "config.h"

// Set to 1 to print:
#define IMUTEST_PRINT_RAW 0		// raw data
#define IMUTEST_PRINT_QUAT 0	// quaternions
#define IMUTEST_PRINT_EULER 0	// euler angles
#define IMUTEST_PRINT_RATES 0	// get and compute rates
#define IMUTEST_PRINT_3DPLOT 1	// data for 3d plotter

MPU9250 mpu;
uint32_t lastPrint;

static void imuTest_printData(void)
{
	if (millis() - lastPrint > 20)
	{
		//println("[MPU] Data");
		#if IMUTEST_PRINT_RAW
		println("A[%d, %d, %d]mg G[%f, %f, %f]deg/s M[%d, %d, %d]mG",
				(int)1000*mpu.ax, (int)1000*mpu.ay, (int)1000*mpu.az,
				mpu.gx, mpu.gy, mpu.gz,
				(int)mpu.mx, (int)mpu.my, (int)mpu.mz);
		#endif

		#if IMUTEST_PRINT_QUAT
		println("Q[%f, %f, %f, %f]", mpu.q[0], mpu.q[1], mpu.q[2], mpu.q[3]);
		#endif

		#if IMUTEST_PRINT_EULER
		println("Y %f P %f R %f", mpu.yaw, mpu.pitch, mpu.roll);
		#endif

		#if IMUTEST_PRINT_3DPLOT
		println("w%fwa%fab%fbc%fc", mpu.q[0], mpu.q[1], mpu.q[2], mpu.q[3]);
		#endif

		lastPrint = millis();
	}
}


static void imuTest_printBiases(void)
{
	println("accel biases [ms]:\t%f\t%f\t%f", 1000.0 * mpu.accelBias[0], 1000.0 * mpu.accelBias[1], 1000.0 * mpu.accelBias[2]);
	println("gyro biases [dps]:\t%f\t%f\t%f", mpu.gyroBias[0], mpu.gyroBias[1], mpu.gyroBias[2]);
	println("mag biases [mG]:\t%f\t%f\t%f", mpu.magBias[0], mpu.magBias[1], mpu.magBias[2]);
	delay(2000);
	/*
accel biases (mg): [3935.302734, 2100.402832, 97.839355]
gyro biases (dps): [88.206108, 3.259542, 312.610687]
mag biases [mG]:        -42.451466      -15.919300      -899.914856
	 */
}

static bool imuTest_begin(void)
{
	mpu.i2c = Get_I2C1_Instance();
	mpu.i2c_addr = MPU9250_I2C_ADDRESS;
	mpu.i2c_addr_ak = AK8963_I2C_ADDRESS;

	writePin(LEDB, HIGH);
	MPU9250_init(&mpu, &mpu9250_default_config);
	writePin(LEDB, LOW);
	writePin(LEDA, HIGH);
	AK8963_init(&mpu, &mpu9250_default_config);
	writePin(LEDA, LOW);
	imuTest_printBiases();
	return false;
}
static bool imuTest_loop(void)
{
	if (MPU9250_update(&mpu))
	{
		imuTest_printData();
		if (mpu.yaw >= 178.0 && mpu.yaw <= 182.0) writePin(LEDA, HIGH);
		else writePin(LEDA, LOW);
	}
}

static void imuTest_printSelfTest(void)
{
	println("MPU9250 Self Test [% of factory value]:");
	//println("Acceleration:\t%f\t%f\t%f", mpu.selfTest[0], mpu.selfTest[1], mpu.selfTest[2]);
	//println("Gyration:\t%f\t%f\t%f", mpu.selfTest[3], mpu.selfTest[4], mpu.selfTest[5]);
}


static void imuTest_printMagTest(void)
{
	println("AK8963 mag biases [mG]:\t%f\t%f\t%f", mpu.magBias[0], mpu.magBias[1], mpu.magBias[2]);
	println("AK8963 Axis sensitivity adjustment values:\t%f\t%f\t%f", mpu.magCalibration[0], mpu.magCalibration[1], mpu.magCalibration[2]);
}




/*
tempCount = MPU_readTempData();  // Read the gyro adc values
temperature = ((float) tempCount) / 333.87 + 21.0; // Gyro chip temperature in degrees Centigrade
// Print temperature in degrees Centigrade
*/
