#include "mpu9250.h"

#include "run.h"
#include "clock.h"
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"

// #################### Low level communication ####################

static void MPU9250_writeByte(MPU9250* inst, uint8_t mem_addr, uint8_t data)
{
	HAL_I2C_Mem_Write(inst->i2c, inst->i2c_addr, mem_addr, 1, &data, 1, 5);
}
static char MPU9250_readByte(MPU9250* inst, uint8_t mem_addr)
{
	uint8_t data[1]; // `data` will store the register data
	HAL_I2C_Mem_Read(inst->i2c, inst->i2c_addr, mem_addr, 1, data, 1, 5);
	return (char)data[0];
}
static void MPU9250_readBytes(MPU9250* inst, uint8_t mem_addr, uint8_t count, uint8_t* dest)
{     
	uint8_t data[14];
	HAL_I2C_Mem_Read(inst->i2c, inst->i2c_addr, mem_addr, 1, data, count, 5);

	int ii = 0;
	for(ii = 0; ii < count; ii++) // maybe memcpy() [?]
	{
		dest[ii] = data[ii];
	}
}
static void AK8963_writeByte(MPU9250* inst, uint8_t mem_addr, uint8_t data)
{
	HAL_I2C_Mem_Write(inst->i2c, inst->i2c_addr_ak, mem_addr, 1, &data, 1, 5);
}
static char AK8963_readByte(MPU9250* inst, uint8_t mem_addr)
{
	uint8_t data[1]; // `data` will store the register data
	HAL_I2C_Mem_Read(inst->i2c, inst->i2c_addr_ak, mem_addr, 1, data, 1, 5);
	return (char)data[0];
}
static void AK8963_readBytes(MPU9250* inst, uint8_t mem_addr, uint8_t count, uint8_t* dest)
{     
	uint8_t data[14];
	HAL_I2C_Mem_Read(inst->i2c, inst->i2c_addr_ak, mem_addr, 1, data, count, 5);

	int ii = 0;
	for(ii = 0; ii < count; ii++) // maybe memcpy() [?]
	{
		dest[ii] = data[ii];
	}
}

static void MPU9250_readAccelData(MPU9250* inst)
{
	uint8_t rawData[6];
	MPU9250_readBytes(inst, MPU9250_ACCEL_XOUT_H, 6, &rawData[0]);
	// calculate actual g value
	inst->ax = (float)((int16_t)(((int16_t)rawData[0] << 8) | rawData[1]))*inst->aRes - inst->accelBias[0];	
	inst->ay = (float)((int16_t)(((int16_t)rawData[2] << 8) | rawData[3]))*inst->aRes - inst->accelBias[1];
	inst->az = (float)((int16_t)(((int16_t)rawData[4] << 8) | rawData[5]))*inst->aRes - inst->accelBias[2];
}
static void MPU9250_readGyroData(MPU9250* inst)
{
	uint8_t rawData[6];
	MPU9250_readBytes(inst, MPU9250_GYRO_XOUT_H, 6, &rawData[0]);
	// calculate actual gyro value
	inst->gx = (float)((int16_t)(((int16_t)rawData[0] << 8) | rawData[1]))*inst->gRes - inst->gyroBias[0];
	inst->gy = (float)((int16_t)(((int16_t)rawData[2] << 8) | rawData[3]))*inst->gRes - inst->gyroBias[1];
	inst->gz = (float)((int16_t)(((int16_t)rawData[4] << 8) | rawData[5]))*inst->gRes - inst->gyroBias[2];
}
static void MPU9250_readMagData(MPU9250* inst)
{
	uint8_t rawData[7];											// 7 because must read ST2 at end of data acquisition
	if(AK8963_readByte(inst, AK8963_ST1) & 0x01)				// wait for magnetometer data ready bit to be set
	{
		AK8963_readBytes(inst, AK8963_XOUT_L, 7, &rawData[0]);	// Read the six raw data and ST2 registers sequentially into data array
		uint8_t c = rawData[6];									// End data read by reading ST2 register
		if(!(c & 0x08))											// Check if magnetic sensor overflow set, if not then report data
		{
			// calculate actual magnetometer value including factory calibration values
			inst->mx = (float)((int16_t)(((int16_t)rawData[1] << 8) | rawData[0]))*inst->mRes*inst->magCalibration[0] - inst->magBias[0];	
			inst->my = (float)((int16_t)(((int16_t)rawData[3] << 8) | rawData[2]))*inst->mRes*inst->magCalibration[1] - inst->magBias[1];
			inst->mz = (float)((int16_t)(((int16_t)rawData[5] << 8) | rawData[4]))*inst->mRes*inst->magCalibration[2] - inst->magBias[2];
		}
	}
}
static void MPU9250_readTempData(MPU9250* inst)
{
	uint8_t rawData[2];
	MPU9250_readBytes(inst, MPU9250_TEMP_OUT_H, 2, &rawData[0]);
	// calculate chip temperature in Celcius degrees
	inst->temperature = ((float)((int16_t)(((int16_t)rawData[0]) << 8 | rawData[1]))) / 333.87 + 21.0;
}
static void MPU9250_getRes(MPU9250* inst, MPU9250_config* config)
{
	switch (config->Mscale)
	{
		case MPU9250_MFS_14BITS:	inst->mRes = 10.0*4912.0/8190.0;	break;
		case MPU9250_MFS_16BITS:	inst->mRes = 10.0*4912.0/32760.0;	break;
	}
	switch (config->Gscale)
	{
		case MPU9250_GFS_250DPS:	inst->gRes = 250.0/32768.0;			break;
		case MPU9250_GFS_500DPS:	inst->gRes = 500.0/32768.0;			break;
		case MPU9250_GFS_1000DPS:	inst->gRes = 1000.0/32768.0;		break;
		case MPU9250_GFS_2000DPS:	inst->gRes = 2000.0/32768.0;		break;
	}
	switch (config->Ascale)
	{
		case MPU9250_AFS_2G:		inst->aRes = 2.0/32768.0;			break;
		case MPU9250_AFS_4G:		inst->aRes = 4.0/32768.0;			break;
		case MPU9250_AFS_8G:		inst->aRes = 8.0/32768.0;			break;
		case MPU9250_AFS_16G:		inst->aRes = 16.0/32768.0;			break;
	}
}

// #################### Public routines ####################

bool MPU9250_init(MPU9250* inst, MPU9250_config* config)
{
	inst->active = false;
	inst->alg_lastUpdate = 0;
	if (config->alg_rate > 0) inst->alg_delay = 1.0 / config->alg_rate;	// calculate delay in [s] from [Hz]
	else inst->alg_delay = 0;
	inst->euler_delay = 1000000.0 / config->euler_rate;
	int i;
	for (i = 0; i < 3; i++)
	{
		inst->eulerOffsets[i] = config->eulerOffsets[i];
		inst->eInt[i] = 0.0f;
		inst->q[i+1] = 0.0f;
	}
	inst->q[0] = 1.0f;

	//Self test was there
	delay(1000);
	MPU9250_getRes(inst, config);			// Get sensor resolutions based on config, only need to do this once

	if (config->calibrate)
	{
		MPU9250_calibrate(inst);			// Calibrate gyro and accelerometers, load biases in bias registers	
	}
	else
	{										// Or load biases from config
		for (i = 0; i < 3; i++)
		{
			inst->accelBias[i] = config->accelBias[i];
			inst->gyroBias[i] = config->gyroBias[i];
			inst->magBias[i] = config->magBias[i];
		}
	}
	delay(1000);
															// Initialize MPU9250 device
															// wake up device
	MPU9250_writeByte(inst, MPU9250_PWR_MGMT_1, 0x00);		// Clear sleep mode bit (6), enable all sensors 
	delay(100); 											// get PLL established on x-axis gyro; should check for PLL ready interrupt  
															// get stable time source
	MPU9250_writeByte(inst, MPU9250_PWR_MGMT_1, 0x01);  	// Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001
															// Configure Gyro and Accelerometer
															// Disable FSYNC and set accelerometer and gyro bandwidth to 44 and 42 Hz, respectively
	delay(100);												// DLPF_CFG = bits 2:0 = 010; this sets the sample rate at 1 kHz for both
	MPU9250_writeByte(inst, MPU9250_CONFIG, 0x03);			// Maximum delay is 4.9 ms which is just over a 200 Hz maximum rate
	delay(100);												// Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV)
	MPU9250_writeByte(inst, MPU9250_SMPLRT_DIV, 0x04);		// Use a 200 Hz rate; the same rate set in CONFIG above

															// Set gyroscope full scale range
															// Range selects FS_SEL and AFS_SEL are 0 - 3
															// so 2-bit values are left-shifted into positions 4:3
	uint8_t c = MPU9250_readByte(inst, MPU9250_GYRO_CONFIG);// get current GYRO_CONFIG register value
	// c = c & ~0xE0; 										// Clear self-test bits [7:5]
	c = c & ~0x03;											// Clear Fchoice bits [1:0]
	c = c & ~0x18;											// Clear AFS bits [4:3]
	c = c | config->Gscale << 3;							// Set full scale range for the gyro
	// c =| 0x00; 											// Set Fchoice for the gyro to 11 by writing its inverse to bits 1:0 of GYRO_CONFIG
	MPU9250_writeByte(inst, MPU9250_GYRO_CONFIG, c);		// Write new GYRO_CONFIG value to register

															// Set accelerometer full-scale range configuration
	c = MPU9250_readByte(inst, MPU9250_ACCEL_CONFIG);		// get current ACCEL_CONFIG register value
	// c = c & ~0xE0;										// Clear self-test bits [7:5] 
	c = c & ~0x18;											// Clear AFS bits [4:3]
	c = c | config->Ascale << 3;							// Set full scale range for the accelerometer
	MPU9250_writeByte(inst, MPU9250_ACCEL_CONFIG, c);		// Write new ACCEL_CONFIG register value

															// Set accelerometer sample rate configuration
															// It is possible to get a 4 kHz sample rate from the accelerometer by choosing 1
															// for accel_fchoice_b bit [3]; in this case the bandwidth is 1.13 kHz
	c = MPU9250_readByte(inst, MPU9250_ACCEL_CONFIG2); 		// get current ACCEL_CONFIG2 register value
	c = c & ~0x0F; 											// Clear accel_fchoice_b (bit 3) and A_DLPFG (bits [2:0])  
	c = c | 0x03;											// Set accelerometer rate to 1 kHz and bandwidth to 41 Hz
	MPU9250_writeByte(inst, MPU9250_ACCEL_CONFIG2, c);		// Write new ACCEL_CONFIG2 register value

	// The accelerometer, gyro, and thermometer are set to 1 kHz sample rates, 
	// but all these rates are further reduced by a factor of 5 to 200 Hz because of the SMPLRT_DIV setting

															// Configure Interrupts and Bypass Enable
	MPU9250_writeByte(inst, MPU9250_INT_PIN_CFG, 0x22);    	// Set interrupt pin active high, push-pull,
															//and clear on read of INT_STATUS, enable I2C_BYPASS_EN
	MPU9250_writeByte(inst, MPU9250_INT_ENABLE, 0x01);		// Enable data ready (bit 0) interrupt

	/*
	if (!config->calibrate && false)						// [SKIPPED] Do this step immidiately only if not calibrating
	{
		AK8963_init(inst, config);	 						// Get magnetometer calibration from AK8963 ROM
	}
	*/

	inst->mpu_active = true;
	inst->active = inst->mpu_active && inst->ak_active;
	return true;
}

bool AK8963_init(MPU9250* inst, MPU9250_config* config)
{
	delay(1000);
															// First extract the factory calibration for each magnetometer axis
	uint8_t rawData[3];  									// x/y/z gyro calibration data stored here
	AK8963_writeByte(inst, AK8963_CNTL, 0x00);				// Power down magnetometer
	delay(10);
	AK8963_writeByte(inst, AK8963_CNTL, 0x0F);				// Enter Fuse ROM access mode
	delay(10);
	AK8963_readBytes(inst, AK8963_ASAX, 3, &rawData[0]);	// Read the x,y,z-axis calibration values
	inst->magCalibration[0] =  (float)(rawData[0] - 128)/256.0f + 1.0f;   // Return axis sensitivity adjustment values
	inst->magCalibration[1] =  (float)(rawData[1] - 128)/256.0f + 1.0f;
	inst->magCalibration[2] =  (float)(rawData[2] - 128)/256.0f + 1.0f;
	AK8963_writeByte(inst, AK8963_CNTL, 0x00);				// Power down magnetometer
	delay(10);
															// Configure the magnetometer for continuous read and highest resolution
															// set Mscale bit 4 to 1 (0) to enable 16 (14) bit resolution in CNTL register,
															// and enable continuous mode data acquisition Mmode (bits [3:0]),
															// 0010 for 8 Hz and 0110 for 100 Hz sample rates
	AK8963_writeByte(inst, AK8963_CNTL, config->Mscale << 4 | config->Mmode); // Set magnetometer data resolution and sample ODR
	delay(10);

	if (config->calibrate)
	{
		AK8963_calibrate(inst);
	}
	else
	{
		int i;
		for (i = 0; i < 3; i++)
		{
			inst->magBias[i] = config->magBias[i];
		}
	}
	
	inst->ak_active = true;
	inst->active = inst->mpu_active && inst->ak_active;
	return true;
}

bool MPU9250_present(MPU9250* inst) { /*MPU9250_reset(inst);*/ return (MPU9250_readByte(inst, MPU9250_WHO_AM_I) == 0x71); }
bool AK8963_present(MPU9250* inst) { /*MPU9250_reset(inst);*/ return (AK8963_readByte(inst, AK8963_WHO_AM_I) == 0x48); }

bool MPU9250_update(MPU9250* inst)
{
	if (MPU9250_readByte(inst, MPU9250_INT_STATUS) & 0x01)	// check if data is ready
	{
		MPU9250_readAccelData(inst);
		MPU9250_readGyroData(inst);
		MPU9250_readMagData(inst);
	}

	inst->alg_deltat = ((micros() - inst->alg_lastUpdate)/1000000.0f);// set integration time by time elapsed since last filter update
	if (inst->alg_deltat >= inst->alg_delay)
	{
		//inst->lastAlgUpdate = micros();	// [!!!] check position of this line vs performance
		MadgwickQuaternionUpdate(inst);
		inst->alg_lastUpdate = micros();

		if (micros() - inst->euler_lastUpdate >= inst->euler_delay)	// ! euler_delay is multiplied to be in [us]
		{
			MPU9250_updateEuler(inst);
			inst->euler_lastUpdate = micros();
			return true;
		}
	}
	return false;
}

void MPU9250_reset(MPU9250* inst)
{
	MPU9250_writeByte(inst, MPU9250_PWR_MGMT_1, 0x80); // Write a one to bit 7 reset bit; toggle reset device
	delay(100);
}

void MPU9250_calibrate(MPU9250* inst)					// Function which accumulates gyro and accelerometer data after device initialization.
{														// It calculates the average of the at-rest readings and
	uint16_t i, packet_count, fifo_count;				// then loads the resulting offsets into accelerometer and gyro bias registers.
	int32_t gyro_bias[3] = {0, 0, 0}, accel_bias[3] = {0, 0, 0};
	uint8_t data[12]; 									// data array to hold accelerometer and gyro x, y, z, data

	MPU9250_reset(inst);								// clear gyro and accelerometer bias registers

	MPU9250_writeByte(inst, MPU9250_PWR_MGMT_1, 0x01);  // Get stable time source
	MPU9250_writeByte(inst, MPU9250_PWR_MGMT_2, 0x00);  // Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001
	delay(200);
														// Configure device for bias calculation
	MPU9250_writeByte(inst, MPU9250_INT_ENABLE, 0x00);  // Disable all interrupts
	MPU9250_writeByte(inst, MPU9250_FIFO_EN, 0x00);     // Disable FIFO
	MPU9250_writeByte(inst, MPU9250_PWR_MGMT_1, 0x00);  // Turn on internal clock source
	MPU9250_writeByte(inst, MPU9250_I2C_MST_CTRL, 0x00);// Disable I2C master
	MPU9250_writeByte(inst, MPU9250_USER_CTRL, 0x00);   // Disable FIFO and I2C master modes
	MPU9250_writeByte(inst, MPU9250_USER_CTRL, 0x0C);   // Reset FIFO and DMP
	delay(15);
														// Configure MPU9250 gyro and accelerometer for bias calculation
	MPU9250_writeByte(inst, MPU9250_CONFIG, 0x01);      // Set low-pass filter to 188 Hz
	MPU9250_writeByte(inst, MPU9250_SMPLRT_DIV, 0x00);  // Set sample rate to 1 kHz
	MPU9250_writeByte(inst, MPU9250_GYRO_CONFIG, 0x00); // Set gyro full-scale to 250 degrees per second, maximum sensitivity
	MPU9250_writeByte(inst, MPU9250_ACCEL_CONFIG, 0x00);// Set accelerometer full-scale to 2 g, maximum sensitivity

	uint16_t  gyrosensitivity  = 131;					// = 131 LSB/degrees/sec
	uint16_t  accelsensitivity = 16384;					// = 16384 LSB/g

														// Configure FIFO to capture accelerometer and gyro data for bias calculation
	MPU9250_writeByte(inst, MPU9250_USER_CTRL, 0x40);   // Enable FIFO
	MPU9250_writeByte(inst, MPU9250_FIFO_EN, 0x78);     // Enable gyro and accelerometer sensors for FIFO (max size 512 bytes in MPU-9250)
	delay(40);											// accumulate 40 samples in 80 milliseconds = 480 bytes

														// At end of sample accumulation, turn off FIFO sensor read
	MPU9250_writeByte(inst, MPU9250_FIFO_EN, 0x00);		// Disable gyro and accelerometer sensors for FIFO
	MPU9250_readBytes(inst, MPU9250_FIFO_COUNTH, 2, &data[0]); // read FIFO sample count
	fifo_count = ((uint16_t)data[0] << 8) | data[1];
	packet_count = fifo_count/12;						// How many sets of full gyro and accelerometer data for averaging

	for (i = 0; i < packet_count; i++)
	{
		int16_t accel_temp[3] = {0, 0, 0}, gyro_temp[3] = {0, 0, 0};	// [!!!] strange, cannot get rid of those arrays, conversion is screwed then
		MPU9250_readBytes(inst, MPU9250_FIFO_R_W, 12, &data[0]); // read data for averaging
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

	for (i = 0; i < 3; i++)								// Normalize sums to get average count biases
	{
		accel_bias[i] /= (int32_t) packet_count;
		gyro_bias[i]  /= (int32_t) packet_count;

		if (i == abs(MPU9250_VERTICAL_AXIS) && false)			// Remove gravity from the z-axis accelerometer bias calculation
		{
			//accel_bias[abs(MPU9250_VERTICAL_AXIS)] += (int32_t)(accelsensitivity * sgn(MPU9250_VERTICAL_AXIS)); // !!! not sure about sign!
																					// [!!!] \/ \/ scarry, no sgn()!
			accel_bias[abs(MPU9250_VERTICAL_AXIS)] -= (int32_t)(accelsensitivity * (accel_bias[abs(MPU9250_VERTICAL_AXIS)] / abs(accel_bias[abs(MPU9250_VERTICAL_AXIS)])));
		}
		//[!!!] this part isn't needed unless [commented STEP1]	// Construct the gyro biases for push to the hardware gyro bias registers,
		data[2*i] =  ((-gyro_bias[i]/4) >> 8) & 0xFF;	// which are reset to zero upon device startup.
		data[2*i+1] = (-gyro_bias[i]/4)		  & 0xFF;	// Divide by 4 to get 32.9 LSB per deg/s to conform to expected bias input format
		//[!!!] goto [commented STEP1]					// Biases are additive, so change sign on calculated average gyro biases
		inst->gyroBias[i] = (float)gyro_bias[i]/(float)gyrosensitivity;	// Construct gyro bias in deg/s for later manual subtraction
	}

	if(accel_bias[2] > 0L)	accel_bias[2] -= (int32_t) accelsensitivity;
	else 						accel_bias[2] += (int32_t) accelsensitivity;

	/* [commented STEP1]:
	MPU9250_writeByte(inst, MPU9250_XG_OFFSET_H, data[0]);	// Push gyro biases to hardware registers
	MPU9250_writeByte(inst, MPU9250_XG_OFFSET_L, data[1]);
	MPU9250_writeByte(inst, MPU9250_YG_OFFSET_H, data[2]);
	MPU9250_writeByte(inst, MPU9250_YG_OFFSET_L, data[3]);
	MPU9250_writeByte(inst, MPU9250_ZG_OFFSET_H, data[4]);
	MPU9250_writeByte(inst, MPU9250_ZG_OFFSET_L, data[5]);
	*/

	// Construct the accelerometer biases for push to the hardware accelerometer bias registers. These registers contain
	// factory trim values which must be added to the calculated accelerometer biases; on boot up these registers will hold
	// non-zero values. In addition, bit 0 of the lower byte must be preserved since it is used for temperature
	// compensation calculations. Accelerometer bias registers expect bias input as 2048 LSB per g, so that
	// the accelerometer biases calculated above must be divided by 8.

	int32_t accel_bias_reg[3] = {0, 0, 0};							// A place to hold the factory accelerometer trim biases
	MPU9250_readBytes(inst, MPU9250_XA_OFFSET_H, 2, &data[0]);		// Read factory accelerometer trim values
	accel_bias_reg[0] = (int16_t)((int16_t)data[0] << 8) | data[1];
	MPU9250_readBytes(inst, MPU9250_YA_OFFSET_H, 2, &data[0]);
	accel_bias_reg[1] = (int16_t)((int16_t)data[0] << 8) | data[1];
	MPU9250_readBytes(inst, MPU9250_ZA_OFFSET_H, 2, &data[0]);
	accel_bias_reg[2] = (int16_t)((int16_t)data[0] << 8) | data[1];

	uint32_t mask = 1uL;						// Define mask for temperature compensation bit 0 of lower byte of accelerometer bias registers
	uint8_t mask_bit[3] = {0, 0, 0};			// Define array to hold mask bit for each accelerometer bias axis
	for(i = 0; i < 3; i++)
	{											// If temperature compensation bit is set, record that fact in mask_bit
		if(accel_bias_reg[i] & mask) mask_bit[i] = 0x01;
												// Construct total accelerometer bias
		accel_bias_reg[i] -= (accel_bias[i]/8);	// Subtract calculated averaged accelerometer bias scaled to 2048 LSB/g (16 g full scale)

		//[!!!] this part isn't needed unless [commented STEP2]
		data[2*i]   = (accel_bias_reg[i] >> 8) & 0xFF;
		data[2*i+1] =  accel_bias_reg[i]	   & 0xFF;
		data[2*i+1] = data[2*i+1] | mask_bit[i];// Preserve temperature compensation bit when writing back to accelerometer bias registers
		//[!!!] goto [commented STEP2]
												// Output scaled accelerometer biases for manual subtraction in the main program
		inst->accelBias[i] = (float)accel_bias[i]/(float)accelsensitivity;
	}

	/* [commented STEP2]:
	// Apparently this is not working for the acceleration biases in the MPU-9250
	// Are we handling the temperature correction bit properly?
	// Push accelerometer biases to hardware registers
	MPU9250_writeByte(inst, MPU9250_XA_OFFSET_H, data[0]);
	MPU9250_writeByte(inst, MPU9250_XA_OFFSET_L, data[1]);
	MPU9250_writeByte(inst, MPU9250_YA_OFFSET_H, data[2]);
	MPU9250_writeByte(inst, MPU9250_YA_OFFSET_L, data[3]);
	MPU9250_writeByte(inst, MPU9250_ZA_OFFSET_H, data[4]);
	MPU9250_writeByte(inst, MPU9250_ZA_OFFSET_L, data[5]);
	*/
}

bool MPU9250_SelfTest(MPU9250* inst)						// Accelerometer and gyroscope self test; check calibration wrt factory settings
{															// percent deviation from factory trim values should be within +/- 14% to pass
	uint8_t selfTest[6];
	float gAvg[3] = {0}, aAvg[3] = {0}, aSTAvg[3] = {0}, gSTAvg[3] = {0};
	float factoryTrim[6];
	uint8_t FS = 0;
	float results[6];

	inst->aRes = 1;											// Prepare instance to SelfTest
	inst->gRes = 1;											// Configuration values (must?) not affect readings
	int i = 0;
	for (i = 0; i < 3; i++)
	{
		inst->accelBias[i] = 0.0f;
		inst->gyroBias[i] = 0.0f;
	}

	MPU9250_writeByte(inst, MPU9250_SMPLRT_DIV, 0x00);		// Set gyro sample rate to 1 kHz
	MPU9250_writeByte(inst, MPU9250_CONFIG, 0x02);			// Set gyro sample rate to 1 kHz and DLPF to 92 Hz
	MPU9250_writeByte(inst, MPU9250_GYRO_CONFIG, FS<<3);	// Set full scale range for the gyro to 250 dps
	MPU9250_writeByte(inst, MPU9250_ACCEL_CONFIG2, 0x02);	// Set accelerometer rate to 1 kHz and bandwidth to 92 Hz
	MPU9250_writeByte(inst, MPU9250_ACCEL_CONFIG, FS<<3);	// Set full scale range for the accelerometer to 2 g

	for(i = 0; i < 200; i++)								// Get average current values of gyro and acclerometer
	{
		MPU9250_readAccelData(inst);
		aAvg[0] += inst->ax;
		aAvg[1] += inst->ay;
		aAvg[2] += inst->az;

		MPU9250_readGyroData(inst);
		gAvg[0] += inst->gx;
		gAvg[1] += inst->gy;
		gAvg[2] += inst->gz;
	}

	for (i = 0; i < 3; i++)									// Get average of 200 values and store as average current readings
	{
		aAvg[i] /= 200;
		gAvg[i] /= 200;
	}
															// Configure the accelerometer for self-test
	MPU9250_writeByte(inst, MPU9250_ACCEL_CONFIG, 0xE0);	// Enable self test on all three axes and set accelerometer range to +/- 2 g
	MPU9250_writeByte(inst, MPU9250_GYRO_CONFIG, 0xE0); 	// Enable self test on all three axes and set gyro range to +/- 250 degrees/s
	delay(25); 												// Delay a while to let the device stabilize

	for(i = 0; i < 200; i++)								// Get average self-test values of gyro and acclerometer
	{
		MPU9250_readAccelData(inst);
		aSTAvg[0] += inst->ax;
		aSTAvg[1] += inst->ay;
		aSTAvg[2] += inst->az;

		MPU9250_readGyroData(inst);
		gSTAvg[0] += inst->gx;
		gSTAvg[1] += inst->gy;
		gSTAvg[2] += inst->gz;
	}

	for (i = 0; i < 3; i++)									// Get average of 200 values and store as average self-test readings
	{
		aSTAvg[i] /= 200;
		gSTAvg[i] /= 200;
	}

	MPU9250_writeByte(inst, MPU9250_ACCEL_CONFIG, 0x00);	// Configure the gyro and accelerometer for normal operation
	MPU9250_writeByte(inst, MPU9250_GYRO_CONFIG, 0x00);
	delay(25); 												// Delay a while to let the device stabilize

	selfTest[0] = MPU9250_readByte(inst, MPU9250_SELF_TEST_X_ACCEL);	// Retrieve accelerometer and gyro 
	selfTest[1] = MPU9250_readByte(inst, MPU9250_SELF_TEST_Y_ACCEL);	// factory Self-Test Code from USR_Reg
	selfTest[2] = MPU9250_readByte(inst, MPU9250_SELF_TEST_Z_ACCEL);	// (self-test results)
	selfTest[3] = MPU9250_readByte(inst, MPU9250_SELF_TEST_X_GYRO);
	selfTest[4] = MPU9250_readByte(inst, MPU9250_SELF_TEST_Y_GYRO);
	selfTest[5] = MPU9250_readByte(inst, MPU9250_SELF_TEST_Z_GYRO);

	for (int i = 0; i < 6; i++)															// Retrieve factory self-test value
	{																					// from self-test code reads
		factoryTrim[i] = (float)(2620/1<<FS)*(pow(1.01, ((float)selfTest[i] - 1.0)));	// (FT[axis] factory trim calculation)
	}

	for (int i = 0; i < 3; i++)								// Report results as a ratio of (STR - FT)/FT
	{														// the change from Factory Trim of the Self-Test Response
		results[i]   = 	100.0*(aSTAvg[i] - aAvg[i])/factoryTrim[i]   - 100.0;
		results[i+3] =	100.0*(gSTAvg[i] - gAvg[i])/factoryTrim[i+3] - 100.0;
	}														// To get percent, must multiply by 100

	#if MPU9250_DEBUG
		println("[MPU9250] Self test results:");
	#endif
	bool passed = true;
	for (int i = 0; i < 6; i++)								// Check if results obey +/-14% bound
	{
		passed &= (results[i] > -14.0) & (results[i] < 14.0);
		#if MPU9250_DEBUG
			println("%f", results[i]);
		#endif
	}
	return passed;
}

void AK8963_calibrate(MPU9250* inst)
{
	uint16_t i = 0, j = 0, sample_count = 0;
	int16_t mag_max[3] = {-32767, -32767, -32767}, mag_min[3] = {32767, 32767, 32767}, mag_temp[3] = {0, 0, 0};

	float temp_mRes = inst->mRes;				// Temporarily store set calibation values
	float temp_magCalibration[3] = {inst->magCalibration[0], inst->magCalibration[1], inst->magCalibration[2]};
	inst->mRes = 1;								// Calibration values (must?) not affect readings
	for (i = 0; i < 3; i++)
	{
		inst->magBias[i] = 0.0f;
		inst->magCalibration[i] = 1.0f;
	}

	delay(4000);								// [!!!] <- WHAT??, must change that

	sample_count = 64;
	for(i = 0; i < sample_count; i++)
	{
		MPU9250_readMagData(inst);				// Read the mag data
		mag_temp[0] = (int16_t)inst->mx;
		mag_temp[1] = (int16_t)inst->my;
		mag_temp[2] = (int16_t)inst->mz;
		for (j = 0; j < 3; j++)
		{
			if(mag_temp[j] > mag_max[j]) mag_max[j] = mag_temp[j];
			if(mag_temp[j] < mag_min[j]) mag_min[j] = mag_temp[j];
		}
		delay(135);  // at 8 Hz ODR, new mag data is available every 125 ms [!!!] <- this to change also
	}

	inst->mRes = temp_mRes;						// Retrieve stored calibation values

	for (i = 0; i < 3; i++)						// get average axis mag bias in counts
	{											// save mag biases in G for main program
		inst->magCalibration[i] = temp_magCalibration[i]; // Retrieve stored calibration values
		inst->magBias[i] = (float)((mag_max[i] + mag_min[i])/2) * inst->mRes * inst->magCalibration[i];
	}
}

// #################### Algotithms ####################


void MPU9250_updateEuler(MPU9250* inst)		// Convert quaternions to Euler angles
{
	inst->yaw   = atan2(2.0f * (inst->q[1] * inst->q[2] + inst->q[0] * inst->q[3]),
						inst->q[0] * inst->q[0] + inst->q[1] * inst->q[1] - inst->q[2] * inst->q[2] - inst->q[3] * inst->q[3]);
	inst->pitch = -asin(2.0f * (inst->q[1] * inst->q[3] - inst->q[0] * inst->q[2]));
	inst->roll  = atan2(2.0f * (inst->q[0] * inst->q[1] + inst->q[2] * inst->q[3]),
						inst->q[0] * inst->q[0] - inst->q[1] * inst->q[1] - inst->q[2] * inst->q[2] + inst->q[3] * inst->q[3]);
	inst->yaw	*= -180.0f / M_PI;			// Convert to degrees
	inst->pitch *= 180.0f / M_PI;
	inst->roll  *= 180.0f / M_PI;

	inst->yaw 	+= inst->eulerOffsets[0];	// Add offsets
	inst->pitch	+= inst->eulerOffsets[1];
	inst->roll 	+= inst->eulerOffsets[2];

	inst->yaw_dx = inst->yaw;
	inst->yaw_dx -= inst->yaw;
	inst->yaw_dx *= (1000000.0f / (micros() - inst->euler_lastUpdate));

	if(inst->yaw < 0)	inst->yaw	+= 360.0f;	// Fix overflow
	else if (inst->yaw > 360) inst->yaw -= 360.0f;
	if(inst->pitch < 0)	inst->pitch	+= 360.0f;
	else if (inst->pitch > 360) inst->pitch -= 360.0f;
	if(inst->roll < 0)	inst->roll	+= 360.0f;
	else if (inst->roll > 360) inst->roll -= 360.0f;

	/* Other alorithm, also calculates linear acceleration
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
}

void MadgwickQuaternionUpdate(MPU9250* inst)
{
	// Transform matrix for proper axis
	float ax = inst->ax, 			 ay = -(inst->ay), 				az = -(inst->az);
	float gx = inst->gx*M_PI/180.0f, gy = -(inst->gy)*M_PI/180.0f, 	gz = -(inst->gz)*M_PI/180.0f;
	float mx = inst->my, 			 my = -(inst->mx),				mz = inst->mz;

	float q1 = inst->q[0], q2 = inst->q[1], q3 = inst->q[2], q4 = inst->q[3];   // short name local variable for readability
	float norm;
	float hx, hy, _2bx, _2bz;
	float s1, s2, s3, s4;
	float qDot1, qDot2, qDot3, qDot4;

	// Auxiliary variables to avoid repeated arithmetic
	float _2q1mx;
	float _2q1my;
	float _2q1mz;
	float _2q2mx;
	float _4bx;
	float _4bz;
	float _2q1 = 2.0f * q1;
	float _2q2 = 2.0f * q2;
	float _2q3 = 2.0f * q3;
	float _2q4 = 2.0f * q4;
	float _2q1q3 = 2.0f * q1 * q3;
	float _2q3q4 = 2.0f * q3 * q4;
	float q1q1 = q1 * q1;
	float q1q2 = q1 * q2;
	float q1q3 = q1 * q3;
	float q1q4 = q1 * q4;
	float q2q2 = q2 * q2;
	float q2q3 = q2 * q3;
	float q2q4 = q2 * q4;
	float q3q3 = q3 * q3;
	float q3q4 = q3 * q4;
	float q4q4 = q4 * q4;

	// Normalise accelerometer measurement
	norm = sqrtf(ax * ax + ay * ay + az * az);
	if (norm == 0.0f) return; // handle NaN
	norm = 1.0f/norm;
	ax *= norm;
	ay *= norm;
	az *= norm;

	// Normalise magnetometer measurement
	norm = sqrtf(mx * mx + my * my + mz * mz);
	if (norm == 0.0f) return; // handle NaN
	norm = 1.0f/norm;
	mx *= norm;
	my *= norm;
	mz *= norm;

	// Reference direction of Earth's magnetic field
	_2q1mx = 2.0f * q1 * mx;
	_2q1my = 2.0f * q1 * my;
	_2q1mz = 2.0f * q1 * mz;
	_2q2mx = 2.0f * q2 * mx;
	hx = mx * q1q1 - _2q1my * q4 + _2q1mz * q3 + mx * q2q2 + _2q2 * my * q3 + _2q2 * mz * q4 - mx * q3q3 - mx * q4q4;
	hy = _2q1mx * q4 + my * q1q1 - _2q1mz * q2 + _2q2mx * q3 - my * q2q2 + my * q3q3 + _2q3 * mz * q4 - my * q4q4;
	_2bx = sqrtf(hx * hx + hy * hy);
	_2bz = -_2q1mx * q3 + _2q1my * q2 + mz * q1q1 + _2q2mx * q4 - mz * q2q2 + _2q3 * my * q4 - mz * q3q3 + mz * q4q4;
	_4bx = 2.0f * _2bx;
	_4bz = 2.0f * _2bz;

	// Gradient decent algorithm corrective step
	s1 = -_2q3 * (2.0f * q2q4 - _2q1q3 - ax) + _2q2 * (2.0f * q1q2 + _2q3q4 - ay) - _2bz * q3 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q4 + _2bz * q2) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q3 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
	s2 = _2q4 * (2.0f * q2q4 - _2q1q3 - ax) + _2q1 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q2 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + _2bz * q4 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q3 + _2bz * q1) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q4 - _4bz * q2) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
	s3 = -_2q1 * (2.0f * q2q4 - _2q1q3 - ax) + _2q4 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q3 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + (-_4bx * q3 - _2bz * q1) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q2 + _2bz * q4) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q1 - _4bz * q3) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
	s4 = _2q2 * (2.0f * q2q4 - _2q1q3 - ax) + _2q3 * (2.0f * q1q2 + _2q3q4 - ay) + (-_4bx * q4 + _2bz * q2) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q1 + _2bz * q3) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q2 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
	norm = sqrtf(s1 * s1 + s2 * s2 + s3 * s3 + s4 * s4);    // normalise step magnitude
	norm = 1.0f/norm;
	s1 *= norm;
	s2 *= norm;
	s3 *= norm;
	s4 *= norm;

	// Compute rate of change of quaternion
	qDot1 = 0.5f * (-q2 * gx - q3 * gy - q4 * gz) - MPU9250_ALG_BETA * s1;
	qDot2 = 0.5f * (q1 * gx + q3 * gz - q4 * gy) - MPU9250_ALG_BETA * s2;
	qDot3 = 0.5f * (q1 * gy - q2 * gz + q4 * gx) - MPU9250_ALG_BETA * s3;
	qDot4 = 0.5f * (q1 * gz + q2 * gy - q3 * gx) - MPU9250_ALG_BETA * s4;

	// Integrate to yield quaternion
	q1 += qDot1 * inst->alg_deltat;
	q2 += qDot2 * inst->alg_deltat;
	q3 += qDot3 * inst->alg_deltat;
	q4 += qDot4 * inst->alg_deltat;
	norm = sqrtf(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4);    // normalise quaternion
	norm = 1.0f/norm;

	inst->q[0] = q1 * norm;
	inst->q[1] = q2 * norm;
	inst->q[2] = q3 * norm;
	inst->q[3] = q4 * norm;
}

void MahonyQuaternionUpdate(MPU9250* inst)	// Similar to Madgwick scheme but uses proportional and integral filtering
{											// on the error between estimated reference vectors and measured ones.
	// Transform matrix for proper axis
	float ax = inst-> ax, 			 ay = -(inst->ay), 				az = -(inst->az);
	float gx = inst->gx*M_PI/180.0f, gy = -(inst->gy)*M_PI/180.0f, 	gz = -(inst->gz)*M_PI/180.0f;
	float mx = inst->my, 			 my = -(inst->mx),				mz = inst->mz;

	float q1 = inst->q[0], q2 = inst->q[1], q3 = inst->q[2], q4 = inst->q[3];   // short name local variable for readability
	float norm;
	float hx, hy, bx, bz;
	float vx, vy, vz, wx, wy, wz;
	float ex, ey, ez;
	float pa, pb, pc;

	// Auxiliary variables to avoid repeated arithmetic
	float q1q1 = q1 * q1;
	float q1q2 = q1 * q2;
	float q1q3 = q1 * q3;
	float q1q4 = q1 * q4;
	float q2q2 = q2 * q2;
	float q2q3 = q2 * q3;
	float q2q4 = q2 * q4;
	float q3q3 = q3 * q3;
	float q3q4 = q3 * q4;
	float q4q4 = q4 * q4;

	// Normalise accelerometer measurement
	norm = sqrtf(ax * ax + ay * ay + az * az);
	if (norm == 0.0f) return; // handle NaN
	norm = 1.0f / norm;        // use reciprocal for division
	ax *= norm;
	ay *= norm;
	az *= norm;

	// Normalise magnetometer measurement
	norm = sqrtf(mx * mx + my * my + mz * mz);
	if (norm == 0.0f) return; // handle NaN
	norm = 1.0f / norm;        // use reciprocal for division
	mx *= norm;
	my *= norm;
	mz *= norm;

	// Reference direction of Earth's magnetic field
	hx = 2.0f * mx * (0.5f - q3q3 - q4q4) + 2.0f * my * (q2q3 - q1q4) + 2.0f * mz * (q2q4 + q1q3);
	hy = 2.0f * mx * (q2q3 + q1q4) + 2.0f * my * (0.5f - q2q2 - q4q4) + 2.0f * mz * (q3q4 - q1q2);
	bx = sqrtf((hx * hx) + (hy * hy));
	bz = 2.0f * mx * (q2q4 - q1q3) + 2.0f * my * (q3q4 + q1q2) + 2.0f * mz * (0.5f - q2q2 - q3q3);

	// Estimated direction of gravity and magnetic field
	vx = 2.0f * (q2q4 - q1q3);
	vy = 2.0f * (q1q2 + q3q4);
	vz = q1q1 - q2q2 - q3q3 + q4q4;
	wx = 2.0f * bx * (0.5f - q3q3 - q4q4) + 2.0f * bz * (q2q4 - q1q3);
	wy = 2.0f * bx * (q2q3 - q1q4) + 2.0f * bz * (q1q2 + q3q4);
	wz = 2.0f * bx * (q1q3 + q2q4) + 2.0f * bz * (0.5f - q2q2 - q3q3);

	// Error is cross product between estimated direction and measured direction of gravity
	ex = (ay * vz - az * vy) + (my * wz - mz * wy);
	ey = (az * vx - ax * vz) + (mz * wx - mx * wz);
	ez = (ax * vy - ay * vx) + (mx * wy - my * wx);
	if (MPU9250_ALG_Ki > 0.0f)
	{
		inst->eInt[0] += ex;      // accumulate integral error
		inst->eInt[1] += ey;
		inst->eInt[2] += ez;
	}
	else
	{
		inst->eInt[0] = 0.0f;     // prevent integral wind up
		inst->eInt[1] = 0.0f;
		inst->eInt[2] = 0.0f;
	}

	// Apply feedback terms
	gx = gx + MPU9250_ALG_Kp * ex + MPU9250_ALG_Ki * inst->eInt[0];
	gy = gy + MPU9250_ALG_Kp * ey + MPU9250_ALG_Ki * inst->eInt[1];
	gz = gz + MPU9250_ALG_Kp * ez + MPU9250_ALG_Ki * inst->eInt[2];

	// Integrate rate of change of quaternion
	pa = q2;
	pb = q3;
	pc = q4;
	q1 = q1 + (-q2 * gx - q3 * gy - q4 * gz) * (0.5f * inst->alg_deltat);
	q2 = pa + (q1 * gx + pb * gz - pc * gy) * (0.5f * inst->alg_deltat);
	q3 = pb + (q1 * gy - pa * gz + pc * gx) * (0.5f * inst->alg_deltat);
	q4 = pc + (q1 * gz + pa * gy - pb * gx) * (0.5f * inst->alg_deltat);

	// Normalise quaternion
	norm = sqrtf(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4);
	norm = 1.0f / norm;
	inst->q[0] = q1 * norm;
	inst->q[1] = q2 * norm;
	inst->q[2] = q3 * norm;
	inst->q[3] = q4 * norm;
}
