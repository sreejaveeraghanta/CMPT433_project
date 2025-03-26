#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// LCD Dimension
#define LCD_WIDTH  240
#define LCD_HEIGHT 240
#define LCD_COLOR_BYTES 2 // 16 bits per pixel
#define LCD_FRAME_BUFFER_SIZE (LCD_WIDTH*LCD_HEIGHT*LCD_COLOR_BYTES)

// Initialize LCD
void LCD_init();

// Display a frame, frame accessed using shared memory
void LCD_displayFrame();

// Display a frame with interlaced mode, to boost fps(not working now...)
void LCD_displayFrameInterlace();

// Return mapped frame buffer address
void* LCD_getFrameBufferAddress();

// Send an custom LCD command
void LCD_command(uint8_t command, uint8_t* param, int len);

// Set LCD brightness, valid value: 0 to 100
// void LCD_setBrightness(int percent);

// Cleanup LCD
void LCD_deinit();

#ifdef __cplusplus
}
#endif
