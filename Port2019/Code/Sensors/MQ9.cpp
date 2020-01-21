#include "Sensors.h"
#include <stdio.h>
#include <math.h>
#include <Sensors/Sensoro.h>

MQ9::MQ9() { ID = 'L'; }

bool MQ9::begin()
{
	Initialized = false;
	minDelay = 30;
	updateDelay = minDelay;
	fileName = "mq9.txt";
	Initialized = true;
	return true;
}

bool MQ9::update()
{
	if (millis() - lastUpdate > updateDelay && Initialized)
	{
		AirQuality = (float)analogRead(A0);
		
		SDbuffer += String(AirQuality)  +" @" + String(millis()) + "\r\n";
		lastUpdate = millis();
		newReading = true;
		return true;
	}
	return false;
}

String MQ9::listReadings()
{
	return "Air quality: " + String(AirQuality);
}
