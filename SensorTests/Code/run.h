#ifndef RUN_H_
#define RUN_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "usbd_cdc_if.h"

static void setup();
static void loop();

static char printBuffer[256];
static inline void print(char* str)
{
	CDC_Transmit_FS((uint8_t*) str, strlen(str));
};

bool bmp280_begin();
bool radio_begin();

#endif /* RUN_H_ */
