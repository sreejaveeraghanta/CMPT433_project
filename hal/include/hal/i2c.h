#ifndef _I2C_H_
#define _I2C_H_

#include <stdint.h>

// Device bus & address
#define I2C_DRV_LINUX_BUS "/dev/i2c-1"
#define I2C_DEVICE_ADC_ADDRESS 0x48
#define I2C_DEVICE_ACCELEROMETER_ADDRESS 0x19

// Register in TLA2024
#define I2C_REG_CONFIGURATION 0x01
#define I2C_REG_DATA 0x00

// Configuration reg contents for continuously sampling different channels
#define I2C_TLA2024_CHANNEL_CONF_0 0x83C2
#define I2C_TLA2024_CHANNEL_CONF_1 0x83D2
#define I2C_TLA2024_CHANNEL_CONF_2 0x83E2
#define I2C_TLA2024_CHANNEL_CONF_3 0x83F2

int I2C_initBus(char* bus, int address);

void I2C_writeReg16(int i2c_file_desc, uint8_t reg_addr, uint16_t value);

uint16_t I2C_readReg16(int i2c_file_desc, uint8_t reg_addr);

void I2C_writeReg8(int i2c_file_desc, uint8_t reg_addr, uint8_t value);

uint8_t I2C_readReg8(int i2c_file_desc, uint8_t reg_addr);

void I2C_closeBus(int fd);

#endif