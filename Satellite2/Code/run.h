#ifndef RUN_H_
#define RUN_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "usbd_cdc_if.h"

static void setup();
static void loop();

static char printBuffer[512];
static inline void print(char* str)
{
	CDC_Transmit_FS((uint8_t*) str, strlen(str));
};

static bool bmp280_begin();
static bool radio_begin();
static void radio_procedure();

static void dio0_IRQ();

#endif /* RUN_H_ */
