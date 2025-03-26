#include "hal/pwm.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define BUFFER_SIZE 32
#define MAX_PEIOD 469754879
#define MIN_FREQ 3

#define DEFAULT_FREQ 10
#define DEFAULT_POWER_PERCENT 80
#define DEFAULT_ENABLE 1

static char s_periodPath[50];
static char s_dutyCyclePath[50];
static char s_enablePath[50];

enum PWMParam {
    PERIOD,
    DUTY_CYCLE,
    ENABLE
};

static long readParam(enum PWMParam param)
{
    FILE* file;
    switch (param) {
        case PERIOD:
            file = fopen(s_periodPath, "r");
            break;
        case DUTY_CYCLE:
            file = fopen(s_dutyCyclePath, "r");
            break;
        case ENABLE:
            file = fopen(s_enablePath, "r");
            break;
        default:
            break;
    }

    if (!file) {
        perror("Error opening file");
        return -1;
    }

    char buffer[BUFFER_SIZE];
    if (!fgets(buffer, sizeof(buffer), file)) {
        perror("Error reading from file");
        fclose(file);
        return -1;
    }

    fclose(file);
    return strtol(buffer, NULL, 10);
}

static void writeParam(enum PWMParam param, long value)
{
    FILE* file;
    switch (param) {
        case PERIOD:
            file = fopen(s_periodPath, "w");
            break;
        case DUTY_CYCLE:
            file = fopen(s_dutyCyclePath, "w");
            break;
        case ENABLE:
            file = fopen(s_enablePath, "w");
            break;
        default:
            break;
    }
    if (!file) {
        perror("Error opening file");
        return;
    }

    if (fprintf(file, "%ld\n", value) < 0) {
        perror("Error writing to file");
    }

    fclose(file);
}

void PWM_init(int pin)
{
    sprintf(s_periodPath, "/dev/hat/pwm/GPIO%d/period", pin);
    sprintf(s_dutyCyclePath, "/dev/hat/pwm/GPIO%d/duty_cycle", pin);
    sprintf(s_enablePath, "/dev/hat/pwm/GPIO%d/enable", pin);

    PWM_setEnable(DEFAULT_ENABLE);
    printf("setenable\n");
    // PWM_setPower(0); // Prevent failing to write period
    // PWM_setFreq(DEFAULT_FREQ);

    // PWM_setPower(DEFAULT_POWER_PERCENT);
    printf("PWM inited\n");
}

long PWM_getFreq()
{
    return 1.0 / (readParam(PERIOD) / 1e9);
}

// duty_cycle cannot be larger than period
void PWM_setFreq(int freq)
{
    if (freq < MIN_FREQ) {
        return;
    }
    long newPeriod = 1.0 / freq * 1e9;
    long newDutyCycle = newPeriod * (DEFAULT_POWER_PERCENT / 100.0);
    if (newPeriod >= MAX_PEIOD) {
        newPeriod = MAX_PEIOD;
    }

    // Handle error case
    long currDutyCycle = readParam(DUTY_CYCLE);
    if (currDutyCycle > newPeriod) {
        writeParam(DUTY_CYCLE, newDutyCycle);
    }

    // Set period
    writeParam(PERIOD, newPeriod);
    // Set duty cycle
    writeParam(DUTY_CYCLE, newDutyCycle);

}

void PWM_setPower(int percent)
{
    // clamp range 0 to 100
    if (percent < 0) percent = 0;
    if (percent > 0) percent = 100;

    long dutyCycle = percent / 100.0 * readParam(PERIOD);
    printf("dutyCycle: %ld\n", dutyCycle);
    writeParam(DUTY_CYCLE, dutyCycle);
}

void PWM_setEnable(int enable)
{
    writeParam(ENABLE, enable);
}

void PWM_deinit()
{
    PWM_setEnable(0);
    printf("LEDEmitter deinited\n");
}