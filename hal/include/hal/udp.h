// This code is a module for controlling a UDP socket 
// Module to read and respond to UDP packets, which are commands 
// to get program statistics. 
// Referenced from the class notes on UDP programming
#ifndef _UDP_H_
#define _UDP_H_
#ifdef __cplusplus
extern "C" {
#endif

#define MAX_LEN 1024

#define PORT 12345

#include <stdbool.h>

void UDP_init(void); //create socket to listen (thread) 
void UDP_deinit(void); // to close all sockets
void UDP_send(float p1_pos_x, float p1_pos_y,
              float ball_pos_x, float ball_pos_y,
              float p2_pos_x, float p2_pos_y,
              int p1_score, int p2_score);
int UDP_recv(void);

#ifdef __cplusplus
}
#endif

#endif