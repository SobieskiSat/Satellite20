#include "Sensors.h"
#include "Arduino.h"
#include <SparkFunMPU9250-DMP.h>
#include "../src/SobieskiSat.h"

using namespace SobieskiSat;

MPU::MPU() { ID = 'M'; }

bool MPU::begin()
{
	Initialized = false;
	if (imu.begin() == INV_SUCCESS)
	{
		imu.dmpBegin(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_GYRO_CAL, 10);
		fileName = "MPU9250.txt";
		minDelay = 0;
		packetSize = 10;
		updateDelay = minDelay;
		Initialized = true;
	}
	//logger.addToBuffer("[" + String(ID) + "] I " + (Initialized == true ? "1" : "0") + " @" + millis() + "\r\n");
	return Initialized;
}

bool MPU::update()
{
	if (millis() - lastUpdate > updateDelay && Initialized && imu.fifoAvailable())
	{
		
		if (imu.dmpUpdateFifo() == INV_SUCCESS)
		{
			imu.update();
			imu.computeEulerAngles();
			Quat[0] = imu.calcQuat(imu.qw);
			Quat[1] = imu.calcQuat(imu.qx);
			Quat[2] = imu.calcQuat(imu.qy);
			Quat[3] = imu.calcQuat(imu.qz);
			Gyro[0] = imu.roll;
			Gyro[1] = imu.pitch;
			Gyro[2] = imu.yaw;
			Accel[0] = imu.calcAccel(imu.ax);
			Accel[1] = imu.calcAccel(imu.ay);
			Accel[2] = imu.calcAccel(imu.az);
			Mag[0] = imu.calcMag(imu.mx);
			Mag[1] = imu.calcMag(imu.my);
			Mag[2] = imu.calcMag(imu.mz);
			
		
				SDbuffer += String(Gyro[0], 7) + " " + String(Gyro[1], 7) + " " + String(Gyro[2], 7) + " ";
				SDbuffer += String(Accel[0], 7) + " " + String(Accel[1], 7) + " " + String(Accel[2], 7) + " ";
				SDbuffer += String(Mag[0], 7) + " " + String(Mag[1], 7) + " " + String(Mag[2], 7) + " ";
				SDbuffer += String(Quat[0], 7) + " " + String(Quat[1], 7) + " " + String(Quat[2], 7) + " " + String(Quat[3], 7) + " @" + String(millis());
				SDbuffer += "\r\n";
			

			lastUpdate = millis();
			
			//logger.addToBuffer(listReadings(), true);
			
			return true;
		}
	}
	return false;
}

String MPU::listReadings()
{
	return "Angles: " + String(Gyro[0], 2) + " " + String(Gyro[1], 2) + " " + String(Gyro[2], 2)
    + " Accels: " + String(Accel[0], 2) + " " + String(Accel[1], 2) + " " + String(Accel[2], 2)
	+ " Mag: " + String(Mag[0], 2) + " " + String(Mag[1], 2) + " " + String(Mag[2], 2)
	+ " Quat: " + String(Quat[0], 7) + " " + String(Quat[1], 7) + " " + String(Quat[2], 7) + " " + String(Quat[3], 7);
}