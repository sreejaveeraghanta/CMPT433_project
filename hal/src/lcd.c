#include "hal/lcd.h"
#include "hal/gpiorw.h"
#include "hal/spi.h"
#include "hal/pwm.h"

#include <stdint.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

// DC high = send data, low = send command
#define MODE_DATA    1
#define MODE_COMMAND 0

// Command
#define COMMAND_CASET  0x2A // Column address set
#define COMMAND_RASET  0x2B // Row address set
#define COMMAND_RAMWR  0x2C // Memory write

#define DC_LINE_NUM  33 // GPIO27 CHIP0
#define RST_LINE_NUM 41 // GPIO22 CHIP0
#define BL_LINE_NUM  18 // GPIO13 CHIP1
#define BACKLIGHT_GPIO 13

static int s_gpioChipFd0 = 0;
static int s_gpioChipFd1 = 0;
static int s_gpioLineFdDc = 0;
static int s_gpioLineFdBl = 0;
static int s_gpioLineFdRst = 0;

static int s_dcMode = -1;
static bool s_isInited = false;

static uint8_t* s_frameBuffer;

static void setPin(int lineFd, int value)
{
    GPIORW_write(lineFd, value);
}

static void sendCommand(uint8_t command)
{
    if (s_dcMode != MODE_COMMAND) {
        setPin(s_gpioLineFdDc, MODE_COMMAND);
        s_dcMode = MODE_COMMAND;
    }
    SPI_sendBytes(&command, 1);
}

static void sendByte(uint8_t data)
{
    if (s_dcMode != MODE_DATA) {
        setPin(s_gpioLineFdDc, MODE_DATA);
        s_dcMode = MODE_DATA;
    }
    SPI_sendBytes(&data, 1);
}

static void sendNBytes(uint8_t* buffer, int n)
{
    if (s_dcMode != MODE_DATA) {
        setPin(s_gpioLineFdDc, MODE_DATA);
        s_dcMode = MODE_DATA;
    }
    SPI_sendBytes(buffer, n);
}

void LCD_init()
{
    if (s_isInited) {
        perror("LCD: module already inited");
        return;
    }

    // Allocate framebuffer
    s_frameBuffer = (uint8_t*)malloc(LCD_FRAME_BUFFER_SIZE);
    if (s_frameBuffer == NULL) {
        perror("LCD: allocate framebuffer failed");
        return;
    }
    // IO init
    s_gpioChipFd0 = GPIORW_open(GPIO_CHIP_PATH_1); 
    s_gpioChipFd1 = GPIORW_open(GPIO_CHIP_PATH_2); 
    s_gpioLineFdDc = GPIORW_requestLine(s_gpioChipFd0, DC_LINE_NUM, true);
    s_gpioLineFdRst = GPIORW_requestLine(s_gpioChipFd0, RST_LINE_NUM, true);
    s_gpioLineFdBl = GPIORW_requestLine(s_gpioChipFd1, BL_LINE_NUM, true);
    SPI_init();
    // PWM_init(BACKLIGHT_GPIO);
    // PWM_setPower(50);

    // Screen init, borrowed from waveshare library
    // Hardware reset
    struct timespec _100ms = {.tv_sec = 0, .tv_nsec = 100 * 1e6};
    setPin(s_gpioLineFdRst, 1);
    nanosleep(&_100ms, NULL);
    setPin(s_gpioLineFdRst, 0);
    nanosleep(&_100ms, NULL);
    setPin(s_gpioLineFdRst, 1);
    nanosleep(&_100ms, NULL);
    
    // LCD_sendCommand(0x01); // Software reset
    // LCD_sendByteData(0x01);
    // nanosleep(&_100ms, NULL); // Required

    setPin(s_gpioLineFdBl, 1); // Backlight on

    sendCommand(0x3A); // Interface Pixel Format 
    sendByte(0x05); // 65k RGB 16bits per pixel

    sendCommand(0x36); 
    sendByte(0x28); // Orientation and BGR

    sendCommand(0xB2); // Porch Setting
    sendByte(0x0C);
    sendByte(0x0C);
    sendByte(0x00);
    sendByte(0x33);
    sendByte(0x33);

    sendCommand(0xB7); // Gate Control
    sendByte(0x35);

    sendCommand(0xBB); // VCOM Setting
    sendByte(0x19);

    sendCommand(0xC0); // LCM Control     
    sendByte(0x2C);

    sendCommand(0xC2); // VDV and VRH Command Enable
    sendByte(0x01);
    sendCommand(0xC3); // VRH Set
    sendByte(0x12);
    sendCommand(0xC4); // VDV Set
    sendByte(0x20);

    sendCommand(0xC6); // Frame Rate Control in Normal Mode
    sendByte(0x0F);
    
    sendCommand(0xD0); // Power Control 1
    sendByte(0xA4);
    sendByte(0xA1);

    sendCommand(0xE0); // Positive Voltage Gamma Control
    sendByte(0xD0);
    sendByte(0x04);
    sendByte(0x0D);
    sendByte(0x11);
    sendByte(0x13);
    sendByte(0x2B);
    sendByte(0x3F);
    sendByte(0x54);
    sendByte(0x4C);
    sendByte(0x18);
    sendByte(0x0D);
    sendByte(0x0B);
    sendByte(0x1F);
    sendByte(0x23);

    sendCommand(0xE1); // Negative Voltage Gamma Control
    sendByte(0xD0);
    sendByte(0x04);
    sendByte(0x0C);
    sendByte(0x11);
    sendByte(0x13);
    sendByte(0x2C);
    sendByte(0x3F);
    sendByte(0x44);
    sendByte(0x51);
    sendByte(0x2F);
    sendByte(0x1F);
    sendByte(0x1F);
    sendByte(0x20);
    sendByte(0x23);

    // sendCommand(0x53); // control
    // sendByte(0b00101100); 

    // sendCommand(0x51); // Set brightness
    // sendByte(0x00); 

    sendCommand(0x21); // Display inversion on
    sendCommand(0x11); // Sleep Out
    sendCommand(0x29); // Display On

    // Refer to ST7789 datasheet P.198-203
    // Set x range
    const uint16_t xStart = 0;
    const uint16_t xEnd = LCD_WIDTH - 1;
    sendCommand(COMMAND_CASET);
    uint8_t xRange[4] = {
        0xFF & (xStart >> 8),
        0xFF & xStart,
        0xFF & (xEnd >> 8),
        0xFF & xEnd
    };
    sendNBytes(xRange, 4);

    // Set y range
    const uint16_t yStart = 0;
    const uint16_t yEnd = LCD_HEIGHT - 1;
    sendCommand(COMMAND_RASET);
    uint8_t yRange[4] = {
        0xFF & (yStart >> 8),
        0xFF & yStart,
        0xFF & (yEnd >> 8),
        0xFF & yEnd
    };
    sendNBytes(yRange, 4);

    s_isInited = true;
    printf("LCD: inited\n");
}

void LCD_displayFrame()
{
    if (!s_isInited) {
        perror("LCD: Can't send frame, module not inited");
    }
    
    // x range and y range was set in init, assume not changed
    // LCD_displayFrameInterlace();
    sendCommand(COMMAND_RAMWR);
    if (s_dcMode != MODE_DATA) {
        setPin(s_gpioLineFdDc, MODE_DATA);
        s_dcMode = MODE_DATA;
    }
    SPI_sendWords((uint32_t*)s_frameBuffer, LCD_FRAME_BUFFER_SIZE);
}

void LCD_displayFrameInterlace()
{
    if (!s_isInited) {
        perror("LCD: Can't send frame interalced, module not inited");
        return;
    }
    // Decide to send odd or even line
    static bool sf_isSendOdd = true;

    // Set x range should be already set

    int i = sf_isSendOdd ? 1 : 0;
    for (; i < LCD_HEIGHT; i += 2) {
        //Send y range
        const uint16_t yStart = i;
        const uint16_t yEnd = i;
        uint8_t yRange[4] = {
            0xFF & (yStart >> 8),
            0xFF & yStart,
            0xFF & (yEnd >> 8),
            0xFF & yEnd
        };
        sendCommand(COMMAND_RASET);
        sendNBytes(yRange, 4);

        sendCommand(COMMAND_RAMWR);
        if (s_dcMode != MODE_DATA) {
            setPin(s_gpioLineFdDc, MODE_DATA);
            s_dcMode = MODE_DATA;
        }
        SPI_sendWords((uint32_t*)(s_frameBuffer + i*LCD_WIDTH*LCD_COLOR_BYTES), LCD_WIDTH*LCD_COLOR_BYTES);
    }
    sf_isSendOdd = !sf_isSendOdd;
}

void LCD_command(uint8_t command, uint8_t* param, int len)
{
    sendCommand(command); 
    sendNBytes(param, len);
}

uint8_t* LCD_getFrameBuffer()
{
    return s_frameBuffer;
}

// void LCD_setBrightness(int percent)
// {
//     PWM_setPower(percent);
// }

void LCD_deinit()
{
    if (!s_isInited) {
        perror("LCD: can't deinit, module not yet inited");
        return;
    }
    SPI_deinit();
    GPIORW_close(s_gpioLineFdBl);
    GPIORW_close(s_gpioLineFdRst);
    GPIORW_close(s_gpioLineFdDc);
    GPIORW_close(s_gpioChipFd1);
    GPIORW_close(s_gpioChipFd0);
    // PWM_deinit();
    free(s_frameBuffer);
    s_isInited = false;
    printf("LCD deinited\n");
}

