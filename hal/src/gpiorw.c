#include "hal/gpiorw.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>

int GPIORW_open(const char* chipPath)
{
    const int fd = open(chipPath, O_RDONLY);
    if (fd < 0) {
        perror("GPIO_open: failed");
    }
    return fd;
}

int GPIORW_requestLine(int chipFd, int lineNumber, bool isOutput)
{
    struct gpio_v2_line_request request;
    memset(&request, 0, sizeof(request));
    request.offsets[0] = lineNumber;
    request.num_lines = 1;
    if (isOutput) request.config.flags = GPIO_V2_LINE_FLAG_OUTPUT;
    else          request.config.flags = GPIO_V2_LINE_FLAG_INPUT;
    strncpy(request.consumer, "LCD", sizeof(request.consumer));

    int ret = ioctl(chipFd, GPIO_V2_GET_LINE_IOCTL, &request);
    if (ret < 0) {
        perror("GPIO_requestLine: failed");
    }
    return request.fd;
}

void GPIORW_write(int lineFd, int value)
{
    struct gpio_v2_line_values lineValues;
    lineValues.mask = 1;
    lineValues.bits = value;

    int ret = ioctl(lineFd, GPIO_V2_LINE_SET_VALUES_IOCTL, &lineValues);
    if (ret < 0) {
        perror("GPIO_write failed");
    }
}

int GPIORW_read(int lineFd)
{
    struct gpio_v2_line_values lineValues;
    lineValues.mask = 1;

    int ret = ioctl(lineFd, GPIO_V2_LINE_GET_VALUES_IOCTL, &lineValues);
    if (ret < 0) {
        perror("GPIO_read failed");
        return -1;
    }
    
    /* Return the bit value (0 or 1) */
    return lineValues.bits;
}


void GPIORW_close(int fd)
{
    if (close(fd) < 0) {
        perror("GPIO_close: failed");
    }
}