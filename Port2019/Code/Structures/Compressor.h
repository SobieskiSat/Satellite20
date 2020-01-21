#ifndef SOBIESKISATLIBRARY_COMPRESSOR_H
#define SOBIESKISATLIBRARY_COMPRESSOR_H

#include <Sensors/Sensoro.h>
#include "DataPacket.h"

namespace SobieskiSat
{
	class Compressor
	{
		public:
		
		void clear();
		void attach(DataPacket packet);
		DataPacket retrieve(String name, int& lenght, float& variable);
		
		int packetSize = 64;
		char data[64];
		bool generateFormat = false;
		String format = "";
		// index of last bit in data
		int index = 0;
		
		private:
		
		DataPacket find(String name, int &startBit, int &endBit);
		
		
	};
}

#endif
