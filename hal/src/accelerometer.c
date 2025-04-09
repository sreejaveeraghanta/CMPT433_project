#include<stdio.h> 
#include<stdlib.h> 
#include<stdbool.h> 
#include<unistd.h> 
#include<assert.h> 
#include "hal/accelerometer.h"
#include "hal/i2c.h"

// This module reads accelerometer values 
static bool isInitialized;
static int i2c_file;

void Accelerometer_init(void) {
  assert(!isInitialized); 
  i2c_file = I2C_initBus(I2C_DRV_LINUX_BUS, I2C_DEVICE_ACCELEROMETER_ADDRESS);
  isInitialized = true;
}

void Accelerometer_deinit(void) {
  assert(isInitialized); 
  isInitialized = false;
  I2C_closeBus(i2c_file);
}

static int read_LH(uint16_t OUT_L, uint16_t OUT_H) {
  I2C_writeReg8(i2c_file, RESET_REGISTER, RESET);
  I2C_writeReg8(i2c_file, CNTRL_REGISTER, CNTRL);
  uint8_t low = I2C_readReg8(i2c_file, OUT_L); 
  uint8_t high = I2C_readReg8(i2c_file, OUT_H); 
  uint16_t value = (high << 8) | low;
  return value/2048;
}

static int Accelerometer_ReadX(void) {
  assert(isInitialized); 
  return read_LH(X_OUT_L, X_OUT_H);
}

static int Accelerometer_ReadY(void) {
  assert(isInitialized); 
  return read_LH(Y_OUT_L, Y_OUT_H);
}

static int Accelerometer_ReadZ(void) {
  assert(isInitialized); 
  return read_LH(Z_OUT_L, Z_OUT_H);
}

int Accelerometer_get_x_axis(void) {
  assert(isInitialized);
  int x = Accelerometer_ReadX();
  int value = 0;
  if (x > 0 && x < 8) {
    value = -x;
  }
  if (x > 23 && x < 31){
    value =31 - x;
  }
  return value;
}

int Accelerometer_get_y_axis(void) {
  assert(isInitialized);
  int value = 0;
  int y = Accelerometer_ReadY();
  int z = Accelerometer_ReadZ();
  if ((y > 0 && y < 8) && (z > 0 && z < 8)) {
    value = -(8-z);
  }
  if ((y > 23 && y < 31) && (z > 0 && z < 8)) {
    value = 8-z;
  }
  return value;

}