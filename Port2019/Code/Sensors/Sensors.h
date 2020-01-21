#ifndef SOBIESKISATLIBRARY_SENSORS_H
#define SOBIESKISATLIBRARY_SENSORS_H

//towrite #include <Adafruit_GPS.h>
#include "Sensor.h"
#include "Clock.h"
#include "Time.h"
#include "Logger.h"
#include "sensirion_arch_config.h"
#include "sps30.h"
//#include <SparkFunMPU9250-DMP.h>

#include "stdbool.h"

namespace SobieskiSat
{

	class BMP280 : public Sensor
	{
		public:
		BMP280();
		
		float Temperature;
		float Pressure;
		float Altitude;
		
		bool begin();
		bool update();
		String listReadings();
		//static bool (*Sensor::sendLog)(String message, Sensor& sender);
		
		private:
		
		short oversampling;
		double dTemperature, dPressure;
		double dig_T1, dig_T2, dig_T3, dig_T4, dig_P1, dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
		double t_fine;
		
		bool startMeasurment();
		char getUnPT(double &uP, double &uT);
		char readInt(char address, double &value);
		char readUInt(char address, double &value);
		char readBytes(unsigned char *values, char length);
		char writeBytes(unsigned char *values, char length);
	};
	
	class DHT22 : public Sensor
	{
		public:
		DHT22();
		DHT22(int pin_);
		
		float Humidity;
		float Temperature;
		String listReadings();
		
		bool begin();
		bool update();
		
		int read2(float* ptemperature, float* phumidity, byte pdata[40]);
		
		private:
		
		long levelTimeout = 5000000; // 500ms
		int pin = 3;
		
		int sample(byte data[40]);
		long levelTime(byte level, int firstWait = 10, int interval = 6);
		byte bits2byte(byte data[8]);
		int parse(byte data[40], short* ptemperature, short* phumidity);
		
	};
	
	class GPS : public Sensor
	{
		public:
		GPS();

		float Latitude = 50.448214;
		float Longitude = 21.796410;
		float Altitude = 1;
		String LastNMEA;
		Clock HardwareClock;
		Time RecievedDate;
		
		bool begin();
		bool update();
		String listReadings();
		
		Adafruit_GPS gps;
		
		private:
		
		bool gpsAnyZero();
		void updateRecievedDate();
	};
	
	class SPS30 : public Sensor
	{
		public:
		SPS30();
		
		float PM1_0;
		float PM2_5;
		float PM4_0;
		float PM10_0;
		
		bool begin();
		bool update();
		String listReadings();
		
		private:
		
		struct sps30_measurement measurement;
		s16 ret;
		
	};
	
	class BAT : public Sensor
	{
		public:
		BAT();
		
		float Reading;
		
		bool begin();
		bool update();
		String listReadings();
		float getLevel();
	};
	
	class PHR : public Sensor
	{
		public:
		PHR();
		
		float Light;
		
		bool begin();
		bool update();
		String listReadings();
	};
	
	class MQ9 : public Sensor
	{
		public:
		MQ9();
		
		float AirQuality;
		
		bool begin();
		bool update();
		String listReadings();
	};
	/*
	class MPU : public Sensor
	{
		public:
		MPU();
		
		float Gyro[3];
		float Accel[3];
		float Mag[3];
		float Quat[4];
		
		bool begin();
		bool update();
		String listReadings();
		
		private:
		
		MPU9250_DMP imu;
		
	};
	*/
	
	
#define BMP280_ADDR 0x76
#define	BMP280_REG_CONTROL 0xF4
#define	BMP280_REG_RESULT_PRESSURE 0xF7
#define BMP280_REG_RESULT_TEMPRERATURE 0xFA
#define	BMP280_COMMAND_TEMPERATURE 0x2E
#define	BMP280_COMMAND_PRESSURE0 0x25  			 
#define	BMP280_COMMAND_PRESSURE1 0x29  			
#define	BMP280_COMMAND_PRESSURE2 0x2D    
#define	BMP280_COMMAND_PRESSURE3 0x31    
#define	BMP280_COMMAND_PRESSURE4 0x5D    
#define	BMP280_COMMAND_OVERSAMPLING_MAX 0xF5

#define simpleDHTCombileError(t, err) ((t << 8) & 0xff00) | (err & 0x00ff)
#define SimpleDHTErrSuccess 0
#define SimpleDHTErrStartLow 0x10
#define SimpleDHTErrStartHigh 0x11
#define SimpleDHTErrDataLow 0x12
#define SimpleDHTErrDataRead 0x13
#define SimpleDHTErrDataEOF 0x14
#define SimpleDHTErrDataChecksum 0x15
#define SimpleDHTErrZeroSamples 0x16
#define SimpleDHTErrNoPin 0x17
};

#endif

/*

	class LM35 : public Sensor
	{
		public:
		LM35();
		LM35(int pin_);
		
		float Temperature;
		
		bool begin();
		bool update();
		float readValue();
		
		private:
		int pin = A1;
	};
*/
