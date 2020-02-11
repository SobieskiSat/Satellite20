#ifndef _SPARKFUN_MPU9250_DMP_H_
#define _SPARKFUN_MPU9250_DMP_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"

// Optimally, these defines would be passed as compiler options, but Arduino
// doesn't give us a great way to do that.
//#define MPU9250
//#define AK8963_SECONDARY
//#define COMPASS_ENABLED

#include "util/inv_mpu.h"
#include "util/inv_mpu_dmp_motion_driver.h"

typedef int inv_error_t;
#define INV_SUCCESS 0
#define INV_ERROR 0x20

enum t_axisOrder {
	X_AXIS, // 0
	Y_AXIS, // 1
	Z_AXIS  // 2
};

// Define's passed to update(), to request a specific sensor (or multiple):
#define UPDATE_ACCEL   (1<<1)
#define UPDATE_GYRO    (1<<2)
#define UPDATE_COMPASS (1<<3)
#define UPDATE_TEMP    (1<<4)

#define INT_ACTIVE_HIGH 0
#define INT_ACTIVE_LOW  1
#define INT_LATCHED     1
#define INT_50US_PULSE  0

#define MAX_DMP_SAMPLE_RATE 200 // Maximum sample rate for the DMP FIFO (200Hz)
#define FIFO_BUFFER_SIZE 512 // Max FIFO buffer size

const signed char defaultOrientation[9] = {
	1, 0, 0,
	0, 1, 0,
	0, 0, 1
};
#define ORIENT_PORTRAIT          0
#define ORIENT_LANDSCAPE         1
#define ORIENT_REVERSE_PORTRAIT  2
#define ORIENT_REVERSE_LANDSCAPE 3

typedef 
{
	int ax, ay, az;
	int gx, gy, gz;
	int mx, my, mz;
	long qw, qx, qy, qz;
	long temperature;
	unsigned long time;
	float pitch, roll, yaw;
	float heading;
} MPU9250;

MPU9250_MPU9250_DMP(MPU9250* inst);

// begin(void) -- Verifies communication with the MPU-9250 and the AK8963,
// and initializes them to the default state:
// All sensors enabled
// Gyro FSR: +/- 2000 dps
// Accel FSR: +/- 2g
// LPF: 42 Hz
// FIFO: 50 Hz, disabled
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_begin(MPU9250* inst);

// setSensors(unsigned char) -- Turn on or off MPU-9250 sensors. Any of the 
// following defines can be combined: INV_XYZ_GYRO, INV_XYZ_ACCEL, 
// INV_XYZ_COMPASS, INV_X_GYRO, INV_Y_GYRO, or INV_Z_GYRO
// Input: Combination of enabled sensors. Unless specified a sensor will be
//  disabled.
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_setSensors(MPU9250* inst, unsigned char sensors);

// setGyroFSR(unsigned short) -- Sets the full-scale range of the gyroscope
// Input: Gyro DPS - 250, 500, 1000, or 2000
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_setGyroFSR(MPU9250* inst, unsigned short fsr);
// getGyroFSR -- Returns the current gyroscope FSR
// Output: Current Gyro DPS - 250, 500, 1000, or 2000
unsigned short MPU9250_getGyroFSR(MPU9250* inst);
// getGyroSens -- Returns current gyroscope sensitivity. The FSR divided by
// the resolution of the sensor (signed 16-bit).
// Output: Currently set gyroscope sensitivity (e.g. 131, 65.5, 32.8, 16.4)
float MPU9250_getGyroSens(MPU9250* inst);

// setAccelFSR(unsigned short) -- Sets the FSR of the accelerometer
// 
// Input: Accel g range - 2, 4, 8, or 16
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_setAccelFSR(MPU9250* inst, unsigned char fsr);
// getAccelFSR -- Returns the current accelerometer FSR
// Output: Current Accel g - 2, 4, 8, or 16
unsigned char MPU9250_getAccelFSR(MPU9250* inst);
// getAccelSens -- Returns current accelerometer sensitivity. The FSR 
// divided by the resolution of the sensor (signed 16-bit).
// Output: Currently set accel sensitivity (e.g. 16384, 8192, 4096, 2048)
unsigned short MPU9250_getAccelSens(MPU9250* inst);

// getMagFSR -- Returns the current magnetometer FSR
// Output: Current mag uT range - +/-1450 uT
unsigned short MPU9250_getMagFSR(MPU9250* inst);
// getMagSens -- Returns current magnetometer sensitivity. The FSR 
// divided by the resolution of the sensor (signed 16-bit).
// Output: Currently set mag sensitivity (e.g. 0.15)
float MPU9250_getMagSens(MPU9250* inst);

// setLPF -- Sets the digital low-pass filter of the accel and gyro.
// Can be any of the following: 188, 98, 42, 20, 10, 5 (value in Hz)
// Input: 188, 98, 42, 20, 10, or 5 (defaults to 5 if incorrectly set)
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_setLPF(MPU9250* inst, unsigned short lpf);
// getLPF -- Returns the set value of the LPF.
//
// Output: 5, 10, 20, 42, 98, or 188 if set. 0 if the LPF is disabled.
unsigned short MPU9250_getLPF(MPU9250* inst);

// setSampleRate -- Set the gyroscope and accelerometer sample rate to a 
// value between 4Hz and 1000Hz (1kHz).
// The library will make an attempt to get as close as possible to the
// requested sample rate.
// Input: Value between 4 and 1000, indicating the desired sample rate
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_setSampleRate(MPU9250* inst, unsigned short rate);
// getSampleRate -- Get the currently set sample rate.
// May differ slightly from what was set in setSampleRate.
// Output: set sample rate of the accel/gyro. A value between 4-1000.
unsigned short MPU9250_getSampleRate(MPU9250* inst);

// setCompassSampleRate -- Set the magnetometer sample rate to a value
// between 1Hz and 100 Hz.
// The library will make an attempt to get as close as possible to the
// requested sample rate.
// Input: Value between 1 and 100, indicating the desired sample rate
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_setCompassSampleRate(MPU9250* inst, unsigned short rate);
// getCompassSampleRate -- Get the currently set magnetometer sample rate.
// May differ slightly from what was set in setCompassSampleRate.
//
// Output: set sample rate of the magnetometer. A value between 1-100
unsigned short MPU9250_getCompassSampleRate(MPU9250* inst);

// dataReady -- checks to see if new accel/gyro data is available.
// (New magnetometer data cannot be checked, as the library runs that sensor 
//  in single-conversion mode.)
// Output: true if new accel/gyro data is available
bool MPU9250_dataReady(MPU9250* inst);

// update -- Reads latest data from the MPU-9250's data registers.
// Sensors to be updated can be set using the [sensors] parameter.
// [sensors] can be any combination of UPDATE_ACCEL, UPDATE_GYRO,
// UPDATE_COMPASS, and UPDATE_TEMP.
// Output: INV_SUCCESS (0) on success, otherwise error
// Note: after a successful update the public sensor variables 
// (e.g. ax, ay, az, gx, gy, gz) will be updated with new data 
inv_error_t MPU9250_update(MPU9250* inst, unsigned char sensors = 
				   UPDATE_ACCEL | UPDATE_GYRO | UPDATE_COMPASS);

// updateAccel, updateGyro, updateCompass, and updateTemperature are 
// called by the update() public method. They read from their respective
// sensor and update the class variable (e.g. ax, ay, az)
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_updateAccel(MPU9250* inst);
inv_error_t MPU9250_updateGyro(MPU9250* inst);
inv_error_t MPU9250_updateCompass(MPU9250* inst);
inv_error_t MPU9250_updateTemperature(MPU9250* inst);

// configureFifo(unsigned char) -- Initialize the FIFO, set it to read from
// a select set of sensors.
// Any of the following defines can be combined for the [sensors] parameter:
// INV_XYZ_GYRO, INV_XYZ_ACCEL, INV_X_GYRO, INV_Y_GYRO, or INV_Z_GYRO
// Input: Combination of sensors to be read into FIFO
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_configureFifo(MPU9250* inst, unsigned char sensors);
// getFifoConfig -- Returns the sensors configured to be read into the FIFO
// Output: combination of INV_XYZ_GYRO, INV_XYZ_ACCEL, INV_Y_GYRO,
//         INV_X_GYRO, or INV_Z_GYRO
unsigned char MPU9250_getFifoConfig(MPU9250* inst);
// fifoAvailable -- Returns the number of bytes currently filled in the FIFO
// Outputs: Number of bytes filled in the FIFO (up to 512)
unsigned short MPU9250_fifoAvailable(MPU9250* inst);
// updateFifo -- Reads from the top of the FIFO, and stores the new data
// in ax, ay, az, gx, gy, or gz (depending on how the FIFO is configured).
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_updateFifo(MPU9250* inst);
// resetFifo -- Resets the FIFO's read/write pointers
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_resetFifo(MPU9250* inst);

// enableInterrupt -- Configure the MPU-9250's interrupt output to indicate
// when new data is ready.
// Input: 0 to disable, >=1 to enable
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_enableInterrupt(MPU9250* inst, unsigned char enable = 1);
// setIntLevel -- Configure the MPU-9250's interrupt to be either active-
// high or active-low.
// Input: 0 for active-high, 1 for active-low
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_setIntLevel(MPU9250* inst, unsigned char active_low);
// setIntLatched -- Configure the MPU-9250's interrupt to latch or operate
// as a 50us pulse.
// Input: 0 for 
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_setIntLatched(MPU9250* inst, unsigned char enable);
// getIntStatus -- Reads the MPU-9250's INT_STATUS register, which can
// indicate what (if anything) caused an interrupt (e.g. FIFO overflow or
// or data read).
// Output: contents of the INT_STATUS register
short MPU9250_getIntStatus(MPU9250* inst);

// dmpBegin -- Initialize the DMP, enable one or more features, and set the FIFO's sample rate
// features can be any one of 
// DMP_FEATURE_TAP -- Tap detection
// DMP_FEATURE_ANDROID_ORIENT -- Orientation (portrait/landscape) detection
// DMP_FEATURE_LP_QUAT -- Accelerometer, low-power quaternion calculation
// DMP_FEATURE_PEDOMETER -- Pedometer (always enabled)
// DMP_FEATURE_6X_LP_QUAT -- 6-axis (accel/gyro) quaternion calculation
// DMP_FEATURE_GYRO_CAL -- Gyroscope calibration (0's out after 8 seconds of no motion)
// DMP_FEATURE_SEND_RAW_ACCEL -- Send raw accelerometer values to FIFO
// DMP_FEATURE_SEND_RAW_GYRO -- Send raw gyroscope values to FIFO
// DMP_FEATURE_SEND_CAL_GYRO -- Send calibrated gyroscop values to FIFO
// fifoRate can be anywhere between 4 and 200Hz.
// Input: OR'd list of features and requested FIFO sampling rate
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_dmpBegin(MPU9250* inst, unsigned short features = 0, unsigned short fifoRate = MAX_DMP_SAMPLE_RATE);

// dmpLoad -- Loads the DMP with 3062-byte image memory. Must be called to begin DMP.
// This function is called by the dmpBegin function.
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_dmpLoad(MPU9250* inst);

// dmpGetFifoRate -- Returns the sample rate of the FIFO
// Output: Set sample rate, in Hz, of the FIFO
unsigned short MPU9250_dmpGetFifoRate(MPU9250* inst);
// dmpSetFiFoRate -- Sets the rate of the FIFO.
// Input: Requested sample rate in Hz (range: 4-200)
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_dmpSetFifoRate(MPU9250* inst, unsigned short rate);

// dmpUpdateFifo -- Reads from the top of the FIFO and fills accelerometer, gyroscope,
// quaternion, and time public variables (depending on how the DMP is configured).
// Should be called whenever an MPU interrupt is detected
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_dmpUpdateFifo(MPU9250* inst); 

// dmpEnableFeatures -- Enable one, or multiple DMP features.
// Input: An OR'd list of features (see dmpBegin)
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_dmpEnableFeatures(MPU9250* inst, unsigned short mask);
// dmpGetEnabledFeatures -- Returns the OR'd list of enabled DMP features
//
// Output: OR'd list of DMP feature's (see dmpBegin for list)
unsigned short MPU9250_dmpGetEnabledFeatures(MPU9250* inst);

// dmpSetTap -- Enable tap detection and configure threshold, tap time, and minimum tap count.
// Inputs: x/y/zThresh - accelerometer threshold on each axis. Range: 0 to 1600. 0 disables tap
//                       detection on that axis. Units are mg/ms.
//         taps - minimum number of taps to create a tap event (Range: 1-4)
//         tapTime - Minimum number of milliseconds between separate taps
//         tapMulti - Maximum number of milliseconds combined taps
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_dmpSetTap(MPU9250* inst, unsigned short xThresh = 250, 
					  unsigned short yThresh = 250, 
					  unsigned short zThresh = 250,
					  unsigned char taps = 1, 
					  unsigned short tapTime = 100,
					  unsigned short tapMulti = 500);
// tapAvailable -- Returns true if a new tap is available
// Output: True if new tap data is available. Cleared on getTapDir or getTapCount.
bool MPU9250_tapAvailable(MPU9250* inst);
// getTapDir -- Returns the tap direction.
// Output: One of the following: TAP_X_UP, TAP_X_DOWN, TAP_Y_UP, TAP_Y_DOWN, TAP_Z_UP,
//         or TAP_Z_DOWN
unsigned char MPU9250_getTapDir(MPU9250* inst);
// getTapCount -- Returns the number of taps in the sensed direction
// Output: Value between 1-8 indicating successive number of taps sensed.
unsigned char MPU9250_getTapCount(MPU9250* inst);

// dmpSetOrientation -- Set orientation matrix, used for orientation sensing.
// Use defaultOrientation matrix as an example input.
// Input: Gyro and accel orientation in body frame (9-byte array)
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_dmpSetOrientation(MPU9250* inst, const signed char * orientationMatrix = defaultOrientation);
// dmpGetOrientation -- Get the orientation, if any.
// Output: If an orientation is detected, one of ORIENT_LANDSCAPE, ORIENT_PORTRAIT,
//         ORIENT_REVERSE_LANDSCAPE, or ORIENT_REVERSE_PORTRAIT.
unsigned char MPU9250_dmpGetOrientation(MPU9250* inst);

// dmpEnable3Quat -- Enable 3-axis quaternion calculation
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_dmpEnable3Quat(MPU9250* inst);

// dmpEnable6Quat -- Enable 6-axis quaternion calculation
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_dmpEnable6Quat(MPU9250* inst);

// dmpGetPedometerSteps -- Get number of steps in pedometer register
// Output: Number of steps sensed
unsigned long MPU9250_dmpGetPedometerSteps(MPU9250* inst);
// dmpSetPedometerSteps -- Set number of steps to a value
// Input: Desired number of steps to begin incrementing from
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_dmpSetPedometerSteps(MPU9250* inst, unsigned long steps);
// dmpGetPedometerTime -- Get number of milliseconds ellapsed over stepping
// Output: Number of milliseconds where steps were detected
unsigned long MPU9250_dmpGetPedometerTime(MPU9250* inst);
// dmpSetPedometerTime -- Set number time to begin incrementing step time counter from
// Input: Desired number of milliseconds
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_dmpSetPedometerTime(MPU9250* inst, unsigned long time);

// dmpSetInterruptMode --
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_dmpSetInterruptMode(MPU9250* inst, unsigned char mode);
// dmpSetGyroBias --
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_dmpSetGyroBias(MPU9250* inst, long * bias);
// dmpSetAccelBias -- 
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_dmpSetAccelBias(MPU9250* inst, long * bias);

// lowPowerAccel --
// Output: INV_SUCCESS (0) on success, otherwise error
inv_error_t MPU9250_lowPowerAccel(MPU9250* inst, unsigned short rate);

// calcAccel -- Convert 16-bit signed acceleration value to g's
float MPU9250_calcAccel(MPU9250* inst, int axis);
// calcGyro -- Convert 16-bit signed gyroscope value to degree's per second
float MPU9250_calcGyro(MPU9250* inst, int axis);
// calcMag -- Convert 16-bit signed magnetometer value to microtesla (uT)
float MPU9250_calcMag(MPU9250* inst, int axis);
// calcQuat -- Convert Q30-format quaternion to a vector between +/- 1
float MPU9250_calcQuat(MPU9250* inst, long axis);

// computeEulerAngles -- Compute euler angles based on most recently read qw, qx, qy, and qz
// Input: boolean indicating whether angle results are presented in degrees or radians
// Output: class variables roll, pitch, and yaw will be updated on exit.	
void MPU9250_computeEulerAngles(MPU9250* inst, bool degrees = true);

// computeCompassHeading -- Compute heading based on most recently read mx, my, and mz values
// Output: class variable heading will be updated on exit
float MPU9250_computeCompassHeading(MPU9250* inst);

// selfTest -- Run gyro and accel self-test.
// Output: Returns bit mask, 1 indicates success. A 0x7 is success on all sensors.
//         Bit pos 0: gyro
//         Bit pos 1: accel
//         Bit pos 2: mag
int MPU9250_selfTest(MPU9250* inst, unsigned char debug = 0);

unsigned short _aSense;
float _gSense, _mSense;

// Convert a QN-format number to a float
float qToFloat(MPU9250* inst, long number, unsigned char q);
unsigned short orientation_row_2_scale(MPU9250* inst, const signed char *row);

#endif // _SPARKFUN_MPU9250_DMP_H_