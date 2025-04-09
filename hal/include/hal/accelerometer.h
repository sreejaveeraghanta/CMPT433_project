#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#ifdef __cplusplus
extern "C" {
#endif

#define CNTRL_REGISTER 0x20
#define RESET_REGISTER 0x21
#define CNTRL 0x57
#define RESET 0x44

#define DEVICE_ADDRESS_ACCELEROMETER 0x19

// This module reads the Accelerometer
#define X_OUT_L 0x28
#define X_OUT_H 0x29
#define Y_OUT_L 0x2A
#define Y_OUT_H 0x2B
#define Z_OUT_L 0x2C
#define Z_OUT_H 0x2D

void Accelerometer_init(void);
void Accelerometer_deinit(void);

int Accelerometer_get_x_axis(void);
int Accelerometer_get_y_axis(void);

#ifdef __cplusplus
}
#endif

#endif

