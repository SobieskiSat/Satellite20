#include <CanSatKit.h>

#define R_MOT 3
#define L_MOT 2

using namespace CanSatKit;

// set radio receiver parameters - see comments below
// remember to set the same radio parameters in
// transmitter and receiver boards!
Radio radio(Pins::Radio::ChipSelect,
            Pins::Radio::DIO0,
            433.0,                  // frequency in MHz
            Bandwidth_125000_Hz,    // bandwidth - check with CanSat regulations to set allowed value
            SpreadingFactor_9,      // see provided presentations to determine which setting is the best
            CodingRate_4_8);        // see provided presentations to determine which setting is the best

byte l_mot;
byte r_mot;

void setup() {
  SerialUSB.begin(115200);

  pinMode(R_MOT, OUTPUT);
  pinMode(L_MOT, OUTPUT);

  // start radio module  
  radio.begin();
}

void loop() {
  for (int i = 0; i < 255; i++)
  {
    analogWrite(L_MOT, i);
    analogWrite(R_MOT, 255 - i);
    delay(5);
  }
  digitalWrite(L_MOT, 0);
  digitalWrite(R_MOT, 0);
  delay(2000);
  
  for (int i = 0; i < 255; i++)
  {
    analogWrite(R_MOT, i);
    analogWrite(L_MOT, 255 - i);
    delay(5);
  }
  digitalWrite(L_MOT, 0);
  digitalWrite(R_MOT, 0);
  delay(2000);
}
