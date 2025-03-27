#include "hal/spi.h"
#include "hal/lcd.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <string.h>

#define SPI_DEVICE "/dev/spidev1.0"
#define SPI_MODE SPI_MODE_0
#define SPI_BITS_PER_WORD 32
#define SPI_SPEED 50000000

#define STATUS_SUCCESS 0
#define STATUS_FAIL (-1)

static int s_fd;
static bool s_isInited = false;

int SPI_init()
{
    if (s_isInited) {
        perror("SPI: module already initialized");
    }
    
    s_fd = open(SPI_DEVICE, O_RDWR);
    if (s_fd < 0) {
        perror("Failed to open SPI device");
        return -1;
    }

    uint8_t mode = SPI_MODE;
    uint8_t bits = SPI_BITS_PER_WORD;
    uint32_t speed = SPI_SPEED;

    if (ioctl(s_fd, SPI_IOC_WR_MODE, &mode) < 0) {
        perror("Failed to set SPI mode");
        return -1;
    }

    if (ioctl(s_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0) {
        perror("Failed to set SPI bits per word");
        return -1;
    }
    printf("Supported SPI bits per word: %d\n", bits);

    if (ioctl(s_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) {
        perror("Failed to set SPI speed");
        return -1;
    }

    s_isInited = true;
    printf("SPI inited\n");
    return STATUS_SUCCESS;
}

int SPI_sendBytes(uint8_t *buf, size_t cnt)
{
    if (!s_isInited) {
        perror("SPI: sendData Stream failed, module not inited");
        return -1;
    }
    struct spi_ioc_transfer tr = {
        .tx_buf = (uintptr_t)buf,
        .rx_buf = (uintptr_t)NULL,
        .len = cnt,
        .speed_hz = SPI_SPEED,
        .bits_per_word = 8,
        .delay_usecs = 0,
    };

    if (ioctl(s_fd, SPI_IOC_MESSAGE(1), &tr) < 0) {
        perror("SPI send bytes transfer failed");
        return -1;
    }
    return 0;
}

static void reorderPixelBuffer(uint32_t* buf32, int numPixels) {
    int numWords = numPixels / 2;
    uint16_t* buf16 = (uint16_t*)buf32;
    for (int i = 0; i < numWords; i++) {
        uint16_t p1 = buf16[2 * i];
        uint16_t p2 = buf16[2 * i + 1];
        buf32[i] = (uint32_t)(p1) << 16 | p2;
    }
}

int SPI_sendWords(uint32_t *buf, size_t cnt)
{
    if (!s_isInited) {
        perror("SPI: sendData Stream failed, module not inited");
        return -1;
    }

    // If we send 32bits words, need to reorder pixels, otherwise the LCD will look wrong
    reorderPixelBuffer(buf, cnt / 2);

    struct spi_ioc_transfer tr = {
        .tx_buf = (uintptr_t)buf,
        .rx_buf = (uintptr_t)NULL,
        .len = cnt,
        .speed_hz = SPI_SPEED,
        .bits_per_word = SPI_BITS_PER_WORD,
        .delay_usecs = 0,
    };

    if (ioctl(s_fd, SPI_IOC_MESSAGE(1), &tr) < 0) {
        perror("SPI send words transfer failed");
        return -1;
    }
    return 0;
}

void SPI_deinit()
{    
    if (!s_isInited) {
        perror("SPI: Deinit fail, module not inited");
        return;
    }
    if (s_fd >= 0) {
        close(s_fd);
    }
    s_isInited = false;
    printf("SPI deinited\n");
}