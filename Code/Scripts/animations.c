#include "run.h"
#include "stm32f4xx_hal_gpio.h"

uint32_t animation_timer;
uint16_t animation_step;
int8_t animation_dir;

static void play_animation(void (*animation)(void), uint32_t duration) { animation_timer = millis(); while (millis() - animation_timer <= duration) { (*animation)(); delay(1); } }

static const void leds_low(void) { writePin(LEDA, LOW); writePin(LEDB, LOW); writePin(LEDC, LOW); writePin(LEDD, LOW); }
static const void leds_high(void) { writePin(LEDA, HIGH); writePin(LEDB, HIGH); writePin(LEDC, HIGH); writePin(LEDD, HIGH); }

static const void leds_bounce(void)
{
	writePin(LEDA, (animation_step >= 50 && animation_step < 150));
	writePin(LEDB, (animation_step >= 150 && animation_step < 250));
	writePin(LEDC, (animation_step >= 250 && animation_step < 350));
	writePin(LEDD, (animation_step >= 350 && animation_step < 450));
	animation_step += animation_dir;
	if (animation_step > 500) { animation_dir *= -1; }
}

static const void leds_fill(void)
{
	writePin(LEDA, (animation_step >= 100));
	writePin(LEDB, (animation_step >= 200));
	writePin(LEDC, (animation_step >= 300));
	writePin(LEDD, (animation_step >= 400));
	animation_step += animation_dir;
	if (animation_step > 500) { animation_dir *= -1; }
}

static const void leds_confirm(void)
{
	writePin(LEDA, (animation_step % 200 > 60));
	writePin(LEDB, (animation_step % 200 > 60));
	writePin(LEDC, (animation_step % 200 > 60));
	writePin(LEDD, (animation_step % 200 > 60));
	animation_step += animation_dir;
	if (animation_step > 500) { animation_dir *= -1; }
}

static const void leds_negate(void)
{
	writePin(LEDA, (animation_step % 30 > 15));
	writePin(LEDB, (animation_step % 30 > 15));
	writePin(LEDC, (animation_step % 30 > 15));
	writePin(LEDD, (animation_step % 30 > 15));
	animation_step += animation_dir;
	if (animation_step > 500) { animation_dir *= -1; }
}
