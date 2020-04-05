#ifndef SENSORS_MPU9250_H
#define SENSORS_MPU9250_H

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "stm32f4xx_hal.h"

#define MPU9250_DEBUG 1	// enable debug messages

// Ascale
#define MPU9250_AFS_2G 	0
#define MPU9250_AFS_4G	1
#define MPU9250_AFS_8G	2
#define MPU9250_AFS_16G	3
// Gscale
#define MPU9250_GFS_250DPS	0
#define MPU9250_GFS_500DPS	1
#define MPU9250_GFS_1000DPS	2
#define MPU9250_GFS_2000DPS	3
// Mscale
#define MPU9250_MFS_14BITS	0	// 0.6 mG per LSB
#define MPU9250_MFS_16BITS	1	// 0.15 mG per LSB
// Mmode - magnetometer data ODR
#define MPU9250_MMODE_8		0x02
#define MPU9250_MMODE_100	0x06

#define MPU9250_VERTICAL_AXIS 2	// 0 - x, 1 - y, 2 - z (accepts negative)

typedef struct // MPU9250_config
{
	float eulerOffsets[3];

	uint8_t Ascale;		// AFS_2G, AFS_4G, AFS_8G, AFS_16G
	uint8_t Gscale;		// GFS_250DPS, GFS_500DPS, GFS_1000DPS, GFS_2000DPS
	uint8_t Mscale;		// MFS_14BITS or MFS_16BITS, 14-bit or 16-bit magnetometer resolution
	uint8_t Mmode;

	bool calibrate;		// force calibration on startup
	float magBias[3];	// Bias corrections preventing drift	
	float gyroBias[3];
	float accelBias[3];

	float alg_rate;		// rate of quaternion filter update, setting to -1 explicitly sets to highest [Hz]
	float euler_rate;	// rate of Euler angle computation [Hz]

} MPU9250_config;


typedef struct // MPU9250
{
	I2C_HandleTypeDef *i2c;
	uint8_t i2c_addr;
	uint8_t i2c_addr_ak;
	bool mpu_active, ak_active, active;
	
	float yaw, pitch, roll;
	float temperature;
	float ax, ay, az, gx, gy, gz, mx, my, mz;	// variables to hold latest sensor data values
	float aRes, gRes, mRes;	// scale resolutions per LSB for the sensors

	float magBias[3];	// Bias corrections preventing drift	
	float gyroBias[3];
	float accelBias[3];
	float magCalibration[3];// Factory mag calibration

	uint32_t euler_delay;
	uint32_t euler_lastUpdate;
	float eulerOffsets[3];

	// parameters for 6 DoF sensor fusion calculations
	uint32_t alg_lastUpdate;	// used to calculate integration interval
	float alg_deltat;		// dt
	float alg_delay;
	float q[4];				// vector to hold quaternion
	float eInt[3];			// vector to hold integral error for Mahony method

} MPU9250;

#define MPU9250_ALG_Kp 2.0f * 5.0f // these are the free parameters in the Mahony filter and fusion scheme, Kp for proportional feedback, Ki for integral
#define MPU9250_ALG_Ki 0.0f
#define MPU9250_ALG_BETA (sqrt(3.0f / 4.0f) * M_PI * (60.0f / 180.0f))
#define MPU9250_ALG_ZETA (sqrt(3.0f / 4.0f) * M_PI * (1.0f / 180.0f))

bool MPU9250_init(MPU9250* inst, MPU9250_config* config);
bool MPU9250_update(MPU9250* inst);
bool MPU9250_present(MPU9250* inst, uint8_t trials);
void MPU9250_reset(MPU9250* inst);
void MPU9250_calibrate(MPU9250* inst);
bool MPU9250_SelfTest(MPU9250* inst);

bool AK8963_init(MPU9250* inst, MPU9250_config* config);
void AK8963_calibrate(MPU9250* inst);

void MPU9250_updateEuler(MPU9250* inst);
void MadgwickQuaternionUpdate(MPU9250* inst);
void MahonyQuaternionUpdate(MPU9250* inst);

// See also MPU-9250 Register Map and Descriptions, Revision 4.0, RM-MPU-9250A-00, Rev. 1.4, 9/9/2013 for registers not listed in
// above document; the MPU9250 and MPU9150 are virtually identical but the latter has a different register map
//
//Magnetometer Registers
#define AK8963_I2C_ADDRESS	(0x0C << 1)
#define AK8963_WHO_AM_I	0x00 // should return	0x48
#define AK8963_INFO		0x01
#define AK8963_ST1		0x02 // data ready status bit 0
#define AK8963_XOUT_L	0x03 // data
#define AK8963_XOUT_H	0x04
#define AK8963_YOUT_L	0x05
#define AK8963_YOUT_H	0x06
#define AK8963_ZOUT_L	0x07
#define AK8963_ZOUT_H	0x08
#define AK8963_ST2		0x09 // Data overflow bit 3 and data read error status bit 2
#define AK8963_CNTL		0x0A // Power down (0000), single-measurement (0001), self-test (1000) and Fuse ROM (1111) modes on bits 3:0
#define AK8963_ASTC		0x0C // Self test control
#define AK8963_I2CDIS	0x0F // I2C disable
#define AK8963_ASAX		0x10 // Fuse ROM x-axis sensitivity adjustment value
#define AK8963_ASAY		0x11 // Fuse ROM y-axis sensitivity adjustment value
#define AK8963_ASAZ		0x12 // Fuse ROM z-axis sensitivity adjustment value

#define MPU9250_SELF_TEST_X_GYRO	0x00
#define MPU9250_SELF_TEST_Y_GYRO	0x01
#define MPU9250_SELF_TEST_Z_GYRO	0x02

/*
#define MPU9250_X_FINE_GAIN     	0x03 // [7:0] fine gain
#define MPU9250_Y_FINE_GAIN     	0x04
#define MPU9250_Z_FINE_GAIN     	0x05
#define MPU9250_XA_OFFSET_H     	0x06 // User-defined trim values for accelerometer
#define MPU9250_XA_OFFSET_L_TC  	0x07
#define MPU9250_YA_OFFSET_H     	0x08
#define MPU9250_YA_OFFSET_L_TC  	0x09
#define MPU9250_ZA_OFFSET_H     	0x0A
#define MPU9250_ZA_OFFSET_L_TC  	0x0B
*/

#define MPU9250_SELF_TEST_X_ACCEL	0x0D
#define MPU9250_SELF_TEST_Y_ACCEL	0x0E
#define MPU9250_SELF_TEST_Z_ACCEL	0x0F

#define MPU9250_SELF_TEST_A		0x10

#define MPU9250_XG_OFFSET_H		0x13 // User-defined trim values for gyroscope
#define MPU9250_XG_OFFSET_L		0x14
#define MPU9250_YG_OFFSET_H		0x15
#define MPU9250_YG_OFFSET_L		0x16
#define MPU9250_ZG_OFFSET_H		0x17
#define MPU9250_ZG_OFFSET_L		0x18
#define MPU9250_SMPLRT_DIV		0x19
#define MPU9250_CONFIG			0x1A
#define MPU9250_GYRO_CONFIG		0x1B
#define MPU9250_ACCEL_CONFIG	0x1C
#define MPU9250_ACCEL_CONFIG2	0x1D
#define MPU9250_LP_ACCEL_ODR	0x1E
#define MPU9250_WOM_THR			0x1F

#define MPU9250_MOT_DUR			0x20 // Duration counter threshold for motion interrupt generation, 1 kHz rate, LSB = 1 ms
#define MPU9250_ZMOT_THR		0x21 // Zero-motion detection threshold bits [7:0]
#define MPU9250_ZRMOT_DUR		0x22 // Duration counter threshold for zero motion interrupt generation, 16 Hz rate, LSB = 64 ms

#define MPU9250_FIFO_EN			0x23
#define MPU9250_I2C_MST_CTRL	0x24
#define MPU9250_I2C_SLV0_ADDR	0x25
#define MPU9250_I2C_SLV0_REG	0x26
#define MPU9250_I2C_SLV0_CTRL	0x27
#define MPU9250_I2C_SLV1_ADDR	0x28
#define MPU9250_I2C_SLV1_REG	0x29
#define MPU9250_I2C_SLV1_CTRL	0x2A
#define MPU9250_I2C_SLV2_ADDR	0x2B
#define MPU9250_I2C_SLV2_REG	0x2C
#define MPU9250_I2C_SLV2_CTRL	0x2D
#define MPU9250_I2C_SLV3_ADDR	0x2E
#define MPU9250_I2C_SLV3_REG	0x2F
#define MPU9250_I2C_SLV3_CTRL	0x30
#define MPU9250_I2C_SLV4_ADDR	0x31
#define MPU9250_I2C_SLV4_REG	0x32
#define MPU9250_I2C_SLV4_DO		0x33
#define MPU9250_I2C_SLV4_CTRL	0x34
#define MPU9250_I2C_SLV4_DI		0x35
#define MPU9250_I2C_MST_STATUS	0x36
#define MPU9250_INT_PIN_CFG		0x37
#define MPU9250_INT_ENABLE		0x38
#define MPU9250_DMP_INT_STATUS	0x39 // Check DMP interrupt
#define MPU9250_INT_STATUS		0x3A
#define MPU9250_ACCEL_XOUT_H	0x3B
#define MPU9250_ACCEL_XOUT_L	0x3C
#define MPU9250_ACCEL_YOUT_H	0x3D
#define MPU9250_ACCEL_YOUT_L	0x3E
#define MPU9250_ACCEL_ZOUT_H	0x3F
#define MPU9250_ACCEL_ZOUT_L	0x40
#define MPU9250_TEMP_OUT_H		0x41
#define MPU9250_TEMP_OUT_L		0x42
#define MPU9250_GYRO_XOUT_H		0x43
#define MPU9250_GYRO_XOUT_L		0x44
#define MPU9250_GYRO_YOUT_H		0x45
#define MPU9250_GYRO_YOUT_L		0x46
#define MPU9250_GYRO_ZOUT_H		0x47
#define MPU9250_GYRO_ZOUT_L		0x48
#define MPU9250_EXT_SENS_DATA_00	0x49
#define MPU9250_EXT_SENS_DATA_01	0x4A
#define MPU9250_EXT_SENS_DATA_02	0x4B
#define MPU9250_EXT_SENS_DATA_03	0x4C
#define MPU9250_EXT_SENS_DATA_04	0x4D
#define MPU9250_EXT_SENS_DATA_05	0x4E
#define MPU9250_EXT_SENS_DATA_06	0x4F
#define MPU9250_EXT_SENS_DATA_07	0x50
#define MPU9250_EXT_SENS_DATA_08	0x51
#define MPU9250_EXT_SENS_DATA_09	0x52
#define MPU9250_EXT_SENS_DATA_10	0x53
#define MPU9250_EXT_SENS_DATA_11	0x54
#define MPU9250_EXT_SENS_DATA_12	0x55
#define MPU9250_EXT_SENS_DATA_13	0x56
#define MPU9250_EXT_SENS_DATA_14	0x57
#define MPU9250_EXT_SENS_DATA_15	0x58
#define MPU9250_EXT_SENS_DATA_16	0x59
#define MPU9250_EXT_SENS_DATA_17	0x5A
#define MPU9250_EXT_SENS_DATA_18	0x5B
#define MPU9250_EXT_SENS_DATA_19	0x5C
#define MPU9250_EXT_SENS_DATA_20	0x5D
#define MPU9250_EXT_SENS_DATA_21	0x5E
#define MPU9250_EXT_SENS_DATA_22	0x5F
#define MPU9250_EXT_SENS_DATA_23	0x60
#define MPU9250_MOT_DETECT_STATUS	0x61
#define MPU9250_I2C_SLV0_DO			0x63
#define MPU9250_I2C_SLV1_DO			0x64
#define MPU9250_I2C_SLV2_DO			0x65
#define MPU9250_I2C_SLV3_DO			0x66
#define MPU9250_I2C_MST_DELAY_CTRL	0x67
#define MPU9250_SIGNAL_PATH_RESET	0x68
#define MPU9250_MOT_DETECT_CTRL		0x69
#define MPU9250_USER_CTRL			0x6A	// Bit 7 enable DMP, bit 3 reset DMP
#define MPU9250_PWR_MGMT_1			0x6B // Device defaults to the SLEEP mode
#define MPU9250_PWR_MGMT_2			0x6C
#define MPU9250_DMP_BANK			0x6D	// Activates a specific bank in the DMP
#define MPU9250_DMP_RW_PNT			0x6E // Set read/write pointer to a specific start address in specified DMP bank
#define MPU9250_DMP_REG				0x6F	// Register in DMP from which to read or to which to write
#define MPU9250_DMP_REG_1			0x70
#define MPU9250_DMP_REG_2			0x71
#define MPU9250_FIFO_COUNTH			0x72
#define MPU9250_FIFO_COUNTL			0x73
#define MPU9250_FIFO_R_W			0x74
#define MPU9250_WHO_AM_I_MPU9250	0x75 // Should return	0x71
#define MPU9250_XA_OFFSET_H			0x77
#define MPU9250_XA_OFFSET_L			0x78
#define MPU9250_YA_OFFSET_H			0x7A
#define MPU9250_YA_OFFSET_L			0x7B
#define MPU9250_ZA_OFFSET_H			0x7D
#define MPU9250_ZA_OFFSET_L			0x7E

#define MPU9250_ADO_STATE 0x01
#define MPU9250_I2C_ADDRESS ((0x68 | MPU9250_ADO_STATE) << 1)
#define MPU9250_WHO_AM_I	0x75

#endif
