// This code is a module for controlling a UDP socket 
// Module to read and respond to UDP packets, which are commands 
// to get program statistics. 
// Referenced from the class notes on UDP programming
#ifndef _UDPCLIENT_H
#define _UDPCLIENT_H
#define MAX_LEN 1024

#define PORT 12345

#include <stdbool.h>

void UDP_Init(void); //create socket to listen (thread) 
void UDP_CleanUp(void); // to close all sockets
void UDP_Receive(void);
bool UDP_wasTerminated(void);

#endif