#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#define JOYSTICK_LOW 20
#define JOYSTICK_HIGH 80

typedef struct{
    int x;
    int y;
} JoystickReading;

extern int Joystick_encDiff;

// Init joystick
void Joystick_init();

// Return a value 0 to 99 in both x and y direction
JoystickReading Joystick_read();

bool Joystick_isPressed();

// Cleanup joystick
void Joystick_deinit();

#ifdef __cplusplus
}
#endif

#endif