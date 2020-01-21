
/*
   Copyright (c) 2018, Sensirion AG
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.

 * * Neither the name of Sensirion AG nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <Arduino.h>

#include "sensirion_uart.h"
#include "sps30.h"


void setup() {
    SerialUSB.begin(115200);

    while (!Serial) {
      delay(100);
    }

    // use built-in LED to show errors
    sensirion_uart_open();
}

void loop() {
    struct sps30_measurement measurement;
    s16 ret;

    int stat = sps30_probe();
    while (stat != 0) {
        SerialUSB.write("probe failed\n");
        delay(1000);
        stat = sps30_probe();
    }

    /* start measurement and wait for 10s to ensure the sensor has a
     * stable flow and possible remaining particles are cleaned out */
    if (sps30_start_measurement() != 0) {
        SerialUSB.write("error starting measurement\n");
    }
    delay(10000);

    while (1) {
        delay(1000);
        ret = sps30_read_measurement(&measurement);

        if (ret < 0) {
          SerialUSB.write("read measurement failed\n");
        } else {
            if (SPS_IS_ERR_STATE(ret)) {
              SerialUSB.write("Measurements may not be accurate\n");
            }
            //SerialUSB.write("PM 1.0: ");
            SerialUSB.print(measurement.mc_1p0, DEC);
            SerialUSB.print("\t");
            //SerialUSB.write("PM 2.5: ");
            SerialUSB.print(measurement.mc_2p5, DEC);
            SerialUSB.print("\t");
            //SerialUSB.write("PM 4.0: ");
            SerialUSB.print(measurement.mc_4p0, DEC);
            SerialUSB.print("\t");
            //SerialUSB.write("PM 10.0: ");
            SerialUSB.print(measurement.mc_10p0, DEC);
            SerialUSB.println("\t");
        }
    }

    sps30_stop_measurement();
    sensirion_uart_close();
}
