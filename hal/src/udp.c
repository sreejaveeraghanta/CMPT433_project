// This module is referenced from the class notes;
// reads and responds to UDP packets through the 12345 port
#include "hal/udp.h"

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
static bool server_ready = false;
static bool terminated = false;
static int player2_dir = 0;
static bool accelerate = false;

static void send_message(char *response)
{
  char message[MAX_LEN];
  snprintf(message, MAX_LEN, response);
  unsigned int sin_len = sizeof(sinRemote);
  sendto(socketDescriptor, message, strlen(message), 0, (struct sockaddr *)&sinRemote, sin_len);
}

static void *listener()
{
  unsigned int sin_len = sizeof(sinRemote);
  char message[MAX_LEN];
  while (active)
  {
    int bytes = recvfrom(socketDescriptor, message, MAX_LEN - 1, 0, (struct sockaddr *)&sinRemote, &sin_len);
    message[bytes] = '\0';
    if (strcmp(message, "ready") == 0){
      server_ready = true;
    }
    if (strcmp(message, "stop") == 0){
      server_ready = false;
    }
    if (strcmp(message, "up") == 0)
    {
      player2_dir = 1;
    }

    if (strcmp(message, "down") == 0)
    {
      player2_dir = -1;
    }

    if (strcmp(message, "idle") == 0)
    {
      player2_dir = 0;
    }

    if (strcmp(message, "accelerate") == 0)
    {
      accelerate = true;
    }
  }
  pthread_exit(NULL);
  return NULL;
}

// From the lecture on UDP programming (LinuxProgramming.c slide deck)
void UDP_init(void)
{
  // define address structure
  struct sockaddr_in sin;
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_port = htons(PORT);

  // create and bind the socket
  socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);
  bind(socketDescriptor, (struct sockaddr *)&sin, sizeof(sin));

  active = true;
  isInitialized = true;
  pthread_create(&listener_thread, NULL, listener, NULL);
  printf("UDP inited\n");
}

void UDP_deinit(void)
{
  active = false;
  isInitialized = false;
  // tell the server to stop
  send_message("quit");
  pthread_cancel(listener_thread);
  pthread_join(listener_thread, NULL);
  close(socketDescriptor);

  terminated = true;
  active = false;
  printf("UDP deinited\n");
}

void UDP_send(float p1_pos_x, float p1_pos_y,
              float ball_pos_x, float ball_pos_y,
              float p2_pos_x, float p2_pos_y,
              int p1_score, int p2_score)
{
  char message[MAX_LEN];
  snprintf(message, MAX_LEN, "%f%s%f%s%f%s%f%s%f%s%f%s%d%s%d", p1_pos_x, ",", p1_pos_y,
           ",", ball_pos_x, ",", ball_pos_y, ",", p2_pos_x, ",", p2_pos_y, ",", p1_score, 
          ",", p2_score);
  unsigned int sin_len = sizeof(sinRemote);
  sendto(socketDescriptor, message, strlen(message), 0, (struct sockaddr *)&sinRemote, sin_len);
}

int UDP_recv(void)
{
  return player2_dir;
}

bool UDP_server_ready(void)
{
  return server_ready;
}

bool UDP_is_accelerate(void) 
{
  return accelerate;
}

void UDP_set_accelerate(void) 
{
  accelerate = false;
}
