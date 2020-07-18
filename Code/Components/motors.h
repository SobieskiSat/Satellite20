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

static uint32_t MAX_PWM_FREQ = 1;

static bool motorsRunning;

void setMotors(float dutyL, float dutyR);
void haltMotors(void);	// assign timeout interrupt to that

void enableMotors(void);
void disableMotors(void);

void setPwmFrequency(uint32_t f_hz);
void setMotorTimeout(uint32_t timeout_ms);
void motorTimeout(void);

#endif /* COMPONENTS_MOTORS_H_ */
