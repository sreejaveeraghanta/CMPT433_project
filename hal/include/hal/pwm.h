#pragma once

// This module expect you have already configured the required overlay and enabled HAT pin
// Not working now
void PWM_init(int pin);
long PWM_getFreq();

// duty_cycle <= period, 0 <= period <= 469,754,879ns, 2.12Hz <= freq
// Must be enabled to write period freqeuncy and duty cycle
void PWM_setFreq(int freq);
void PWM_setPower(int percent);
void PWM_setEnable(int enable);

void PWM_deinit();