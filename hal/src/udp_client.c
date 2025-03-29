// This module is referenced from the class notes; 
// reads and responds to UDP packets through the 12345 port
#include "hal/udp_client.h"

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <assert.h>
#include <stdbool.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "hal/joystick.h"
#include <string.h>
#include <pthread.h>


static int socketDescriptor;
static struct sockaddr_in sinRemote; 

static bool isInitialized;
pthread_t listener_thread;
static bool active;
static bool terminated = false;

static void send_message(char *response) {
  char message[MAX_LEN];
  snprintf(message, MAX_LEN, response);
  unsigned int sin_len = sizeof(sinRemote); 
  sendto(socketDescriptor, message, strlen(message), 0, (struct sockaddr*)&sinRemote, sin_len);
}

// From the lecture on UDP programming (LinuxProgramming.c slide deck)
void UDP_Init(void) {
  // define address structure
  struct sockaddr_in sin; 
  memset(&sin, 0, sizeof(sin)); 
  sin.sin_family = AF_INET; 
  sin.sin_addr.s_addr = htonl(INADDR_ANY); 
  sin.sin_port = htons(PORT);

  //create and bind the socket
  socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0); 
  bind (socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));

  active = true;
  isInitialized = true;
}

void UDP_CleanUp(void) {
  active = false;
  isInitialized = false;
  send_message("Program was terminated, cannot send any more commands\n");
  pthread_cancel(listener_thread);
  pthread_join(listener_thread, NULL);
  close(socketDescriptor);
}

static void* listener() {
  unsigned int sin_len = sizeof(sinRemote); 
  char message[MAX_LEN]; 
  while(active){
    int bytes = recvfrom(socketDescriptor, message, MAX_LEN - 1, 0, (struct sockaddr *) &sinRemote, &sin_len); 
    message[bytes] = '\0'; 

    JoystickReading reading = Joystick_read();

    if (reading.y > JOYSTICK_HIGH) {
        send_message("UP");
    }

    if (reading.y < JOYSTICK_LOW) {
        send_message("DOWN");
    }

    if (reading.x > JOYSTICK_HIGH) {
        send_message("RIGHT");
    }

    if (reading.y < JOYSTICK_LOW) {
        send_message("LEFT");
    }

    if (strcmp(message, "stop") == 0){
      active = false; 
      terminated = true;
    }

  }
  pthread_exit(NULL);
  return NULL;
}

void UDP_Receive(void) {
  assert(isInitialized);
  pthread_create(&listener_thread, NULL, listener, NULL);
}

bool UDP_wasTerminated(void) {
  return terminated;
}
