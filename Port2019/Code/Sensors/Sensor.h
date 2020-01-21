#ifndef SOBIESKISATLIBRARY_SENSOR_H
#define SOBIESKISATLIBRARY_SENSOR_H

#include "stdbool.h"

class Sensor
{
	public:

	bool Initialized = false;
	String SDbuffer;
	String fileName;
	char ID;
	bool newReading = false;
	//bool newLog = true;
	//bool packetReady = false;

	bool begin();
	bool update();
	bool setUpdateDelay(int ms);
	void SDbufferClear();
	String listReadings();
	//void loadToSDBuffer(std::initializer_list<float> inputs);
	static bool (*sendLog)(String message, Sensor& sender);

	protected:

	long lastUpdate;
	int updateDelay;
	int minDelay;
	int packetSize;
	int packetCount;
	//bool IsPacketReady();
};

#endif
