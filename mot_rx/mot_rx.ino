#include <CanSatKit.h>

#define R_MOT 1
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
  // prepare empty space for received frame
  // maximum length is maximum frame length + null termination
  // 255 + 1 byte = 256 bytes
  char data[256];

  // receive data and save it to string
  radio.receive(data);
  
  // get and print signal level (rssi)
  SerialUSB.print("Received (RSSI = ");
  SerialUSB.print(radio.get_rssi_last());
  SerialUSB.print("): ");

  // print received message
  //SerialUSB.println(data);
  l_mot = data[0];
  r_mot = data[1];

  SerialUSB.print("rRX: ");
  SerialUSB.print(int(l_mot));
  SerialUSB.print(" ");
  SerialUSB.println(int(r_mot));


  analogWrite(LED_BUILTIN, l_mot);
  analogWrite(L_MOT, l_mot);
  analogWrite(R_MOT, r_mot);
}
