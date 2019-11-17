#include <CanSatKit.h>

using namespace CanSatKit;

bool led_state = false;
const int led_pin = 13;

Radio radio(Pins::Radio::ChipSelect,
            Pins::Radio::DIO0,
            433.0,
            Bandwidth_125000_Hz,
            SpreadingFactor_9,
            CodingRate_4_8);

// create (empty) radio frame object that can store data
// to be sent via radio
Frame frame;

int ind = 0;
byte motVal[2] = { 0x00 , 0x00 };
byte incomingByte = 0;

void setup() {
  SerialUSB.begin(115200);
  pinMode(led_pin, OUTPUT);

  radio.begin();
}

void loop() {
  if (SerialUSB.available() > 0 && radio.tx_fifo_empty())
  { 
     incomingByte = SerialUSB.read();
     SerialUSB.print("cRX: ");
     SerialUSB.println(int(incomingByte));

     if (incomingByte == 0x0A)
     {
      digitalWrite(led_pin, led_state);
      led_state = !led_state;
    
      frame.print(char(motVal[0]));
      frame.print(char(motVal[1]));
      radio.transmit(frame);
    
      // print frame also on SerialUSB
      SerialUSB.print("rTX: ");
      SerialUSB.println(frame);
      
      frame.clear();
      ind = 0;
    }
    else
    {
      if (ind < 2)
      {
        motVal[ind] = incomingByte;
        SerialUSB.print("Set: ");
        SerialUSB.print(incomingByte);
        SerialUSB.print(" in ");
        SerialUSB.println(ind);
        ind++;
      }
      else SerialUSB.println("out of index");
    }
  }
}
