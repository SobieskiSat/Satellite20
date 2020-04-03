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

// Set to 1 to print:
#define IMUTEST_PRINT_RAW 0		// raw data
#define IMUTEST_PRINT_QUAT 0	// quaternions
#define IMUTEST_PRINT_EULER 0	// euler angles
#define IMUTEST_PRINT_RATES 0	// get and compute rates
#define IMUTEST_PRINT_3DPLOT 0	// data for 3d plotter
#define IMUTEST_PRINT_FREQ 10UL	// printing frequency in Hz

MPU9250 mpu;

static float sum;
uint32_t sumCount;
uint32_t lastPrint;
uint32_t lastCompute;

float a12, a22, a31, a32, a33;            // rotation matrix coefficients for Euler angles and gravity components
float lin_ax, lin_ay, lin_az;             // linear acceleration (acceleration with gravity component subtracted)

static bool imuTest_getData(void)
{
	if (MPU_readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)	// check if data ready interrupt
	{
		MPU_readAccelData(accelCount);						// Read the x/y/z adc values

		// Now we'll calculate the accleration value into actual g's
		ax = (float)accelCount[0]*aRes - MPU9250accelBias[0];	// get actual g value, this depends on scale being set
		ay = (float)accelCount[1]*aRes - MPU9250accelBias[1];
		az = (float)accelCount[2]*aRes - MPU9250accelBias[2];

		MPU_readGyroData(gyroCount);	// Read the x/y/z adc values

		// Calculate the gyro value into actual degrees per second
		gx = (float)gyroCount[0]*gRes;	// get actual gyro value, this depends on scale being set
		gy = (float)gyroCount[1]*gRes;
		gz = (float)gyroCount[2]*gRes;

		MPU_readMagData(magCount);	// Read the x/y/z adc values

		// Calculate the magnetometer values in milliGauss
		// Include factory calibration per data sheet and user environmental corrections
		mx = (float)magCount[0]*mRes*magCalibration[0] - MPU9250magBias[0];	// get actual magnetometer value, this depends on scale being set
		my = (float)magCount[1]*mRes*magCalibration[1] - MPU9250magBias[1];
		mz = (float)magCount[2]*mRes*magCalibration[2] - MPU9250magBias[2];

		return true;
	}
	return false;
}
static bool imuTest_quatUpdate(void)
{
	if (micros() - lastCompute > 10)
	{
		Now = micros();
		deltat = ((Now - lastUpdate)/1000000.0f); // set integration time by time elapsed since last filter update
		lastUpdate = Now;

		sum += deltat; // sum for averaging filter update rate
		sumCount++;

		MadgwickQuaternionUpdate(ax, -ay, -az, gx*PI/180.0f, -gy*PI/180.0f, -gz*PI/180.0f,  my,  -mx, mz);

		lastCompute = micros();
		return true;
	}
	return false;
}
static void imuTest_getEuler()
{
	yaw   = atan2(2.0f * (q[1] * q[2] + q[0] * q[3]), q[0] * q[0] + q[1] * q[1] - q[2] * q[2] - q[3] * q[3]);
	pitch = -asin(2.0f * (q[1] * q[3] - q[0] * q[2]));
	roll  = atan2(2.0f * (q[0] * q[1] + q[2] * q[3]), q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3]);
	pitch *= 180.0f / PI;
	yaw   *= 180.0f / PI;
	//yaw   -= 5.78f;
	roll  *= 180.0f / PI;
	if(yaw < 0) yaw   += 360.0f;
}
static void imuTest_printData(void)
{
	if (millis() - lastPrint > 20)
	{
		println("[MPU] Data");
		#if IMUTEST_PRINT_RAW
		println("A[%d, %d, %d]mg G[%f, %f, %f]deg/s M[%d, %d, %d]mG",
				(int)1000*ax, (int)1000*ay, (int)1000*az,
				gx, gy, gz,
				(int)mx, (int)my, (int)mz);
		#endif

		#if IMUTEST_PRINT_QUAT
		println("Q[%f, %f, %f, %f]", mpu.q[0], mpu.q[1], mpu.q[2], mpu.q[3]);
		#endif

		#if IMUTEST_PRINT_EULER
		println("Y %f P %f R %f", mpu.yaw, mpu.pitch, mpu.roll);
		#endif

		#if IMUTEST_3DPLOT
		println("w%fwa%fab%fbc%fc", mpu.q[0], mpu.q[1], mpu.q[2], mpu.q[3]);
		#endif

		lastPrint = millis();
	}
}

static bool imuTest_begin(void)
{
	mpu.i2c = Get_I2C1_Instance();
	mpu.i2c_addr = MPU9250_ADDRESS;
	mpu.i2c_addr_ak = AK8963_ADDRESS;
	MPU9250_init(&mpu, &mpu9250_default_config);
	// LED blink blink - for move
	AK8963_init(magCalibration); 
	// LED blink blink - for move
	return false;
}
static bool imuTest_loop(void)
{
	imuTest_getData();
	if (imuTest_quatUpdate()) imuTest_getEuler();
	imuTest_printData();
}

static void imuTest_printSelfTest(void)
{
	println("MPU9250 Self Test [% of factory value]:");
	println("Acceleration:\t%f\t%f\t%f", mpu.SelfTest[0], mpu.SelfTest[1], mpu.SelfTest[2]);
	println("Gyration:\t%f\t%f\t%f", mpu.SelfTest[3], mpu.SelfTest[4], mpu.SelfTest[5]);
}

static void imuTest_printMagTest(void)
{
	println("AK8963 mag biases [mG]:\t%f\t%f\t%f", mpu.config.magBias[0], mpu.config.magBias[1], mpu.config.magBias[2]);
	println("AK8963 Axis sensitivity adjustment values:\t%f\t%f\t%f", mpu.magCalibration[0], mpu.magCalibration[1], mpu.magCalibration[2]);
}


/*
a12 =   2.0f * (q[1] * q[2] + q[0] * q[3]);
a22 =   q[0] * q[0] + q[1] * q[1] - q[2] * q[2] - q[3] * q[3];
a31 =   2.0f * (q[0] * q[1] + q[2] * q[3]);
a32 =   2.0f * (q[1] * q[3] - q[0] * q[2]);
a33 =   q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3];
pitch = -asinf(a32);
roll  = atan2f(a31, a33);
yaw   = atan2f(a12, a22);
pitch *= 180.0f / PI;
yaw   *= 180.0f / PI;
yaw   += 5.78f;
if(yaw < 0) yaw   += 360.0f; // Ensure yaw stays between 0 and 360
roll  *= 180.0f / PI;
lin_ax = ax + a31;
lin_ay = ay + a32;
lin_az = az - a33;
*/

/*
tempCount = MPU_readTempData();  // Read the gyro adc values
temperature = ((float) tempCount) / 333.87 + 21.0; // Gyro chip temperature in degrees Centigrade
// Print temperature in degrees Centigrade
*/
