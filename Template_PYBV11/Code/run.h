#ifndef RUN_H_
#define RUN_H_

#include "fatfs.h"

void setup();
void loop();

static uint32_t printLen = 0;
static char printBuffer[512];

void LED_animation();
void static println(char* str);
FRESULT AppendToFile(char* path, size_t path_len, char* msg, size_t msg_len);
void BlinkLED(uint32_t blink_delay, uint8_t num_blinks);


#endif /* RUN_H_ */
