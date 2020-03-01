// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//		A code to test MPU9250
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdbool.h>

#include "motors.h"
#include "run.h"
#include "mpu9250.h"
#include "clock.h"

// Set to 1 to print:
#define IMUTEST_PRINT_RAW 0		// raw data
#define IMUTEST_PRINT_QUAT 0	// quaternions
#define IMUTEST_PRINT_EULER 0	// euler angles
#define IMUTEST_PRINT_RATES 0	// get and compute rates
#define IMUTEST_PRINT_3DPLOT 0	// data for 3d plotter
#define IMUTEST_PRINT_FREQ 10UL	// printing frequency in Hz

float SelfTest[6];
float MPU9250gyroBias[3];
float MPU9250accelBias[3];
float MPU9250magBias[3];      // Bias corrections for gyro and accelerometer
float sum;
uint32_t sumCount;
uint32_t lastPrint;
uint32_t lastCompute;

float a12, a22, a31, a32, a33;            // rotation matrix coefficients for Euler angles and gravity components
float lin_ax, lin_ay, lin_az;             // linear acceleration (acceleration with gravity component subtracted)

bool imuActive;

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
		print("A[");
		print_int((int)1000*ax); print(", ");
		print_int((int)1000*ay); print(", ");
		print_int((int)1000*az); print("]mg ");
		print("G[");
		print_float(gx); print(", ");
		print_float(gy); print(", ");
		print_float(gz); print("]deg/s ");
		print("M[");
		print_int((int)mx);
		print_int((int)my);
		print_int((int)mz); println("]mG");
		#endif

		#if IMUTEST_PRINT_QUAT
		print("Q[");
		print_float(q[0]); print(", ");
		print_float(q[1]); print(", ");
		print_float(q[2]); print(", ");
		print_float(q[3]); println("]");
		#endif

		#if IMUTEST_PRINT_EULER
		print("Y "); print_float(yaw);
		print("P "); print_float(pitch);
		print("R "); print_float(roll); println("");
		#endif

		#if IMUTEST_3DPLOT
		print("w"); print_float(q[0]); print("wa"); print_float(q[1]); print("ab"); print_float(q[2]); print("bc"); print_float(q[3]); println("c");
		#endif

		lastPrint = millis();
	}
}

static bool imuTest_begin(void)
{
	imuActive = false;

	println("imuTest start!");
	GyroMeasError = PI * (60.0f / 180.0f);
	beta = sqrt(3.0f / 4.0f) * GyroMeasError;
	GyroMeasDrift = PI * (1.0f / 180.0f);
	zeta = sqrt(3.0f / 4.0f) * GyroMeasDrift;

	AAscale = AFS_2G;
	GGscale = GFS_250DPS;
	MMscale = MFS_16BITS;
	Mmode = 0x06;
	magCalibration[0] = 0;
	magCalibration[1] = 0;
	magCalibration[2] = 0;
	magbias[0] = 0;
	magbias[1] = 0;
	magbias[2] = 0;
	delt_t = 0;
	count = 0;
	deltat = 0.0f;
	lastUpdate = 0;
	firstUpdate = 0;
	Now = 0;
	q[0] = 1.0f;
	q[1] = 0.0f;
	q[2] = 0.0f;
	q[3] = 0.0f;
	eInt[0] = 0.0f;
	eInt[1] = 0.0f;
	eInt[2] = 0.0f;


	if (MPU_present())
	{
		HAL_Delay(100);
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

		HAL_GPIO_WritePin(LEDA_GPIO_Port, LEDA_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LEDC_GPIO_Port, LEDC_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LEDD_GPIO_Port, LEDD_Pin, GPIO_PIN_SET);

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

		HAL_GPIO_WritePin(LEDA_GPIO_Port, LEDA_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LEDC_GPIO_Port, LEDC_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LEDD_GPIO_Port, LEDD_Pin, GPIO_PIN_RESET);

		imuActive = true;
		return true;
	}
	return false;
}
static bool imuTest_loop(void)
{
	imuTest_getData();
	if (imuTest_quatUpdate()) imuTest_getEuler();
	imuTest_printData();
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
