#ifndef COMPONENTS_MOTORS_H_
#define COMPONENTS_MOTORS_H_

#include <stdint.h>
#include <stdbool.h>

// Left motor: Timer2 Channel2
// Right motor: Timer2 Channel4
// PWM configured in Mode2 - Up-Down
// DRV8838 has max of 250kHz PWM frequency
// Motors will automatically turn off after (TIM3->ARR * 1ms) time, default 100ms
// Any new value applied will reset timeout timer

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
