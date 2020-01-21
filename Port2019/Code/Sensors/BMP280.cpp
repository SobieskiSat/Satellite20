#include "Sensors.h"
#include <stdio.h>
#include <math.h>
#include <Sensors/Sensoro.h>

using namespace SobieskiSat;

BMP280::BMP280() { ID = 'B'; }

bool BMP280::begin()
{
	Initialized = false;
	Wire.begin();
	fileName = "BMP280.txt";
	oversampling = 4;
	
	if (readUInt(0x88, dig_T1) &&
		readInt(0x8A, dig_T2)  &&
		readInt(0x8C, dig_T3)  &&
		readUInt(0x8E, dig_P1) &&
		readInt(0x90, dig_P2)  &&
		readInt(0x92, dig_P3)  &&
		readInt(0x94, dig_P4)  &&
		readInt(0x96, dig_P5)  &&
		readInt(0x98, dig_P6)  &&
		readInt(0x9A, dig_P7)  &&
		readInt(0x9C, dig_P8)  &&
		readInt(0x9E, dig_P9))
	{
		startMeasurment();
		updateDelay = minDelay;
		packetSize = 10;
		
		Initialized = true;
	}
	//logger.addToBuffer("[" + String(ID) + "] I " + (Initialized == true ? "1" : "0") + " @" + millis() + "\r\n");
	return Initialized;
}

bool BMP280::update()
{
	if (millis() - lastUpdate > updateDelay && Initialized)
	{
		startMeasurment();
		double uP, uT;
		char result = getUnPT(uP,uT);
		if(result!=0)
		{
			// calculate temperature
			double var1 = (uT/16384.0 - dig_T1/1024.0)*dig_T2;
			double var2 = ((uT/131072.0 - dig_T1/8192.0)*(uT/131072.0 - dig_T1/8192.0))*dig_T3;
			t_fine = var1+var2;
			dTemperature = (var1+var2)/5120.0;
			
			// calculate pressure
			var1 = (t_fine/2.0) - 64000.0;
			var2 = var1 * (var1 * dig_P6/32768.0);	//not overflow
			var2 = var2 + (var1 * dig_P5 * 2.0);	//overflow
			var2 = (var2/4.0)+((dig_P4)*65536.0);
		
			var1 = (dig_P3 * var1 * var1/524288.0 + dig_P2 * var1) / 524288.0;
			var1 = (1.0 + var1/32768.0) * dig_P1;
		
			dPressure = 1048576.0- uP;
			dPressure = (dPressure-(var2/4096.0))*6250.0/var1 ;	//overflow	
			var1 = dig_P9 * dPressure * dPressure / 2147483648.0;	//overflow
			var2 = dPressure * dig_P8 / 32768.0;
			dPressure = dPressure + (var1+var2+dig_P7)/16.0;
			dPressure = dPressure / 100.0;
			
			Temperature = (float)dTemperature;
			Pressure = (float)dPressure;
			Altitude = 44330 * (1.0 - pow(Pressure / 1013.25, 0.1903));
			
				SDbuffer += String(Pressure, 7) + " " + String(Temperature, 7) + " @" + String(millis());
				SDbuffer += "\r\n";
			lastUpdate = millis();
			
			//(*Sensor::sendLog)(listReadings(), *this);
			newReading = true;
			return true;
		}
	}
	return false;
}

String BMP280::listReadings()
{
	return "Pressure: " + String(Pressure, 3) + " Temperature: " + String(Temperature, 3);
}

bool BMP280::startMeasurment()
{
	unsigned char data[2], result;
	data[0] = BMP280_REG_CONTROL;

	switch (oversampling)
	{
		case 0:
			data[1] = BMP280_COMMAND_PRESSURE0;
			minDelay = 8;			
			break;
		case 1:
			data[1] = BMP280_COMMAND_PRESSURE1;
			minDelay = 10;			
			break;
		case 2:
			data[1] = BMP280_COMMAND_PRESSURE2;
			minDelay = 15;
			break;
		case 3:
			data[1] = BMP280_COMMAND_PRESSURE3;
			minDelay = 24;
			break;
		case 4:
			data[1] = BMP280_COMMAND_PRESSURE4;
			minDelay = 45;
			break;
		case 16:
			data[1] = BMP280_COMMAND_OVERSAMPLING_MAX;
			minDelay = 80;
			break;
		default:
			data[1] = BMP280_COMMAND_PRESSURE0;
			minDelay = 8;
			break;
	}
	result = writeBytes(data, 2);
	if (result) return true; // everything fine
	else return false; // there was a problem communicating with the BMP
}

char BMP280::getUnPT(double &uP, double &uT)
{
	unsigned char data[6];
	char result;
	
	data[0] = BMP280_REG_RESULT_PRESSURE; //0xF7 

	result = readBytes(data, 6); // 0xF7; xF8, 0xF9, 0xFA, 0xFB, 0xFC
	if (result) // good read
	{
		uP = (double)(data[0] *4096 + data[1]*16 + data[2]/16) ;	//20bit UP
		uT = (double)(data[3]*4096 + data[4]*16 + data[5]/16) ;	//20bit UT
	}
	return(result);
}
char BMP280::readInt(char address, double &value)
{
	unsigned char data[2];	//char is 4bit,1byte

	data[0] = address;
	if (readBytes(data,2))
	{
		value = (double)(int16_t)(((unsigned int)data[1]<<8)|(unsigned int)data[0]); //
		return(1);
	}
	value = 0;
	return(0);
}
char BMP280::readUInt(char address, double &value)
{
	unsigned char data[2];	//4bit
	data[0] = address;
	if (readBytes(data,2))
	{
		value = (double)(unsigned int)(((unsigned int)data[1]<<8)|(unsigned int)data[0]);
		return(1);
	}
	value = 0;
	return(0);
}
char BMP280::readBytes(unsigned char *values, char length)
{
	Wire.beginTransmission(BMP280_ADDR);
	Wire.write(values[0]);
	if (Wire.endTransmission() == 0)
	{
		Wire.requestFrom(BMP280_ADDR,length);
		while(Wire.available() != length) ; // wait until bytes are ready
		for (uint8_t x=0;x<length;x++)
		{
			values[x] = Wire.read();
		}
		return(1);
	}
	return(0);
}
char BMP280::writeBytes(unsigned char *values, char length)
{
	Wire.beginTransmission(BMP280_ADDR);
	Wire.write(values,length);
	if (Wire.endTransmission() == 0)
		return(1);
	else
		return(0);
}
