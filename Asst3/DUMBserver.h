#ifndef _server_h
#define _server_h
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

struct messages {
  char* msg;
  struct messages* next;
};

struct mailBox {
  char* name;
  struct mailBox* next;
  struct messages* first;
  pthread_mutex_t mutex;
};

struct thread_vars {
  char* IP;
  int socket;
};
#endif
