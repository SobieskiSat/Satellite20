#ifndef COMPONENTS_MOTORS_H_
#define COMPONENTS_MOTORS_H_

#include <stdint.h>
#include <stdbool.h>

// Left motor: Timer2 Channel2
// Right motor: Timer2 Channel4
// DRV8838 has max of 250kHz PWM frequency
// PWM frequency = 42MHz / (PSC+1) = 42MHz / 420 = 100kHz

static uint8_t motL = 0;
static uint8_t motR = 0;
static bool motL_forward = true;
static bool motR_forward = true;

static void setMotors(uint8_t pwmL, uint8_t pwmR);
static void haltMotors();	// assign timeout interrupt to that

static void enableMotors();
static void disableMotors();

static void setPwmFrequency(uint32_t f_hz);

#endif /* COMPONENTS_MOTORS_H_ */
