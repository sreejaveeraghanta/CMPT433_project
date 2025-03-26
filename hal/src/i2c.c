#include "hal/i2c.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

int I2C_initBus(char* bus, int address)
{
    int i2c_file_desc = open(bus, O_RDWR);
    if (i2c_file_desc == -1) {
        printf("I2C DRV: Unable to open bus for read/write (%s)\n", bus);
        perror("Error is:");
        exit(EXIT_FAILURE);
    }
    if (ioctl(i2c_file_desc, I2C_SLAVE, address) == -1) {
        perror("Unable to set I2C device to slave address.");
        exit(EXIT_FAILURE);
    }
    return i2c_file_desc;
}

void I2C_writeReg16(int i2c_file_desc, uint8_t reg_addr, uint16_t value)
{
    int tx_size = 1 + sizeof(value);
    uint8_t buff[tx_size];
    buff[0] = reg_addr;
    buff[1] = (value & 0xFF);
    buff[2] = (value & 0xFF00) >> 8;
    int bytes_written = write(i2c_file_desc, buff, tx_size);
    if (bytes_written != tx_size) {
        perror("Unable to write i2c register");
        exit(EXIT_FAILURE);
    }
}

uint16_t I2C_readReg16(int i2c_file_desc, uint8_t reg_addr)
{
    // To read a register, must first write the address
    int bytes_written = write(i2c_file_desc, &reg_addr, sizeof(reg_addr));
    if (bytes_written != sizeof(reg_addr)) {
        perror("Unable to write i2c register.");
        exit(EXIT_FAILURE);
    }
    // Now read the value and return it
    uint16_t raw_read = 0;
    int bytes_read = read(i2c_file_desc, &raw_read, sizeof(raw_read));
    if (bytes_read != sizeof(raw_read)) {
        perror("Unable to read i2c register");
        exit(EXIT_FAILURE);
    }

    // Convert byte order and shift bits into place
    uint16_t value = ((raw_read & 0xFF) << 8) | ((raw_read & 0xFF00) >> 8);
    value  = value >> 4;
    return value;
}

void I2C_writeReg8(int i2c_file_desc, uint8_t reg_addr, uint8_t value)
{
    int tx_size = 1 + sizeof(value);
    uint8_t buff[tx_size];
    buff[0] = reg_addr;
    buff[1] = value;
    int bytes_written = write(i2c_file_desc, buff, tx_size);
    if (bytes_written != tx_size) {
        perror("Unable to write i2c register");
        exit(EXIT_FAILURE);
    }
}


uint8_t I2C_readReg8(int i2c_file_desc, uint8_t reg_addr)
{
    int bytes_written = write(i2c_file_desc, &reg_addr, sizeof(reg_addr));
    if (bytes_written != sizeof(reg_addr)) {
        perror("Unable to write i2c register.");
        exit(EXIT_FAILURE);
    }
    uint8_t raw_read = 0;
    int bytes_read = read(i2c_file_desc, &raw_read, sizeof(raw_read));
    if (bytes_read != sizeof(raw_read)) {
        perror("Unable to read i2c register");
        exit(EXIT_FAILURE);
    }
    return raw_read;
}

void I2C_closeBus(int fd) {
    if (close(fd) == -1) {
        perror("Error closing I2C file descriptor");
        exit(EXIT_FAILURE);
    }
}