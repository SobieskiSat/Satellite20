#ifndef SOBIESKISATLIBRARY_DATAPACKET_H
#define SOBIESKISATLIBRARY_DATAPACKET_H

#include <math.h>

class DataPacket
{
	public:
	DataPacket(String name_, float bottom_limit, float upper_limit, int numbers_, int decimals_, float value_)
	{
		name = name_;
		bottomLimit = bottom_limit;
		upperLimit = upper_limit;
		numbers = numbers_;
		decimals = decimals_;
		value = value_;
		/*
		if (value > upperLimit)
		{
			SerialUSB.println(name + " is above range, assigned overflow value.");
			value = upperLimit - value_ + bottomLimit;
			//state = State.overflow;
		}
		else if (value < bottomLimit)
		{
			SerialUSB.println(name + " is below range, assigned overflow value.");
			value = upperLimit - (bottomLimit - value);
			//state = State.overflow;
		}
		//else state = State.normal;
		*/
	};
		
	String name;
	float bottomLimit;
	float upperLimit;
	int numbers;
	int decimals;
	float value;
		
	//enum State { normal, overflow } state;
};

#endif
