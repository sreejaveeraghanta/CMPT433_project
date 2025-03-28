#pragma once

#include <cstdint>

// A wrapper of printf, include time and log level
void debugLog(const char* format, ...);

void convertRGBAtoRGB565(uint8_t *rgba, uint16_t *rgb565, int width, int height);

bool testEGLError(const char* functionLastCalled);

bool testGLError(const char* functionLastCalled);