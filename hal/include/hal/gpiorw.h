#pragma once

// GPIO read write module
#include <stdbool.h>

#define GPIO_CHIP_PATH_0 "/dev/gpiochip0"
#define GPIO_CHIP_PATH_1 "/dev/gpiochip1"
#define GPIO_CHIP_PATH_2 "/dev/gpiochip2"

// Get GPIO chip file descriptor
int GPIORW_open(const char* chipPath);

// Set GPIO line as output, return GPIO line file descriptor
int GPIORW_requestLine(int chipFd, int lineNumber, bool isOutput);

// Write to GRIO
void GPIORW_write(int lineFd, int value);

// Read GPIO (0/1)
int GPIORW_read(int lineFd);

// Close chip/ line file descriptor
void GPIORW_close(int fd);