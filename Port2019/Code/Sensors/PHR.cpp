#include "Sensors.h"
#include <stdio.h>
#include <math.h>
#include <Sensors/Sensoro.h>

using namespace SobieskiSat;

PHR::PHR() { ID = 'L'; }

bool PHR::begin()
{
	Initialized = false;
	minDelay = 30;
	updateDelay = minDelay;
	fileName = "photo.txt";
	Initialized = true;
	return true;
}

bool PHR::update()
{
	if (millis() - lastUpdate > updateDelay && Initialized)
	{
		Light = (float)analogRead(A2);
		
		SDbuffer += String(Light) +" @" + String(millis()) + "\r\n";
		lastUpdate = millis();
		newReading = true;
		return true;
	}
	return false;
}

String PHR::listReadings()
{
	return "Light intensity: " + String(Light);
}
