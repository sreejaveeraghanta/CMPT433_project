#include "hal/joystick.h"
#include "hal/i2c.h"
#include "hal/gpiorw.h"

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <signal.h>

// Calibration
#define X_MIN 0
#define X_MAX 1620
#define Y_MIN 29
#define Y_MAX 1643

#define MAPPED_DEFAULT_VALUE 50
#define MAPPED_MIN 0
#define MAPPED_MAX 99

#define GPIO_LINE_JOYSTICK_BUTTON 15 // Chip2 GPIO5 Line15

static int    s_gpioFd ;
static int    s_gpioLineFd;
static int    s_i2cFd;
static bool   s_isInited = false;

static int mapReading(uint16_t raw, uint16_t minInput, uint16_t maxInput, int minOutput, int maxOutput) {
    return (raw - minInput) * (maxOutput - minOutput) / (maxInput - minInput) + minOutput;
}

void Joystick_init()
{
    s_gpioFd = GPIORW_open(GPIO_CHIP_PATH_2);
    s_gpioLineFd = GPIORW_requestLine(s_gpioFd, GPIO_LINE_JOYSTICK_BUTTON, false);
    s_i2cFd = I2C_initBus(I2C_DRV_LINUX_BUS, I2C_DEVICE_ADC_ADDRESS);

    s_isInited = true;
    printf("Joystick inited\n");
}

JoystickReading Joystick_read()
{
    // x direction
    I2C_writeReg16(s_i2cFd, I2C_REG_CONFIGURATION, I2C_TLA2024_CHANNEL_CONF_0);
    uint16_t xReading = I2C_readReg16(s_i2cFd, I2C_REG_DATA);
    int mappedReadingX = mapReading(xReading, X_MIN, X_MAX, MAPPED_MIN, MAPPED_MAX);
    if (mappedReadingX < MAPPED_MIN) {
        mappedReadingX = MAPPED_MIN;
    } else if (mappedReadingX > MAPPED_MAX) {
        mappedReadingX = MAPPED_MAX;
    }

    // y diretion
    I2C_writeReg16(s_i2cFd, I2C_REG_CONFIGURATION, I2C_TLA2024_CHANNEL_CONF_1);
    uint16_t yReading = I2C_readReg16(s_i2cFd, I2C_REG_DATA);
    int mappedReadingY = MAPPED_MAX - mapReading(yReading, Y_MIN, Y_MAX, MAPPED_MIN, MAPPED_MAX); // invert
    if (mappedReadingY < MAPPED_MIN) {
        mappedReadingY =  MAPPED_MIN;
    } else if (mappedReadingY > MAPPED_MAX) {
        mappedReadingY =  MAPPED_MAX;
    }

    JoystickReading reading = {
        .x = mappedReadingX,
        .y = mappedReadingY
    };
    return reading;
}

bool Joystick_isPressed()
{
    // For GPIO5 pressed is 0, not pressed is 1
    return !GPIORW_read(s_gpioLineFd);
}

void Joystick_deinit()
{
    if (!s_isInited) {
        printf("Joystick deinited failed\n");
    }

    I2C_closeBus(s_i2cFd);
    GPIORW_close(s_gpioLineFd);
    GPIORW_close(s_gpioFd);
    printf("Joystick deinited\n");
}