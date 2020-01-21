#ifndef SOBIESKISATLIBRARY_CLOCK_H
#define SOBIESKISATLIBRARY_CLOCK_H

#include "Time.h"

class Clock
{
	public:
	void begin(char* initializer);
	void begin(Time initializer);

	Time Start;
	Time Current;
	long Tick;
		
	void update();
		
	private:
	long lastTick;
};

#endif
