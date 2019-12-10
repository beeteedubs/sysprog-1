#include <stdio.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include "DUMBserver.h"
#include <pthread.h>

struct mailBox* head;

void getBoxNames() {
  struct mailBox* temp = head;
  while(temp != NULL) {
    printf("%s\n", temp->name);
    temp = temp->next;
  }
}
char* getName(char* buffer) {
  char* target = (char*) malloc(26 * sizeof(char));
  int i = 0;
  while(buffer[i] != ' '){
    i++;
  }
  i++;
  int targetCounter = 0;
  while(buffer[i] != '\0') {
    target[targetCounter] = buffer[i];
    targetCounter++;
    i++;
  }
  target[targetCounter] = '\0';
  printf("%d\n", targetCounter);
  return target;
}

void CREATE(int socket, char* name) {
  printf("creating...\n");
  if(!(isalpha(name[0]))) {
    send(socket, "ER:WHAT?", sizeof("ER:WHAT?"),0);
    return;
  }
  int exists = 0;
  struct mailBox* temp = head;
  struct mailBox* prev;
  while(temp != NULL) {
    if(strcmp(temp->name, name) == 0) {
      printf("are u in here \n");
      exists = 1;
      break;
    }
    prev = temp;
    temp = temp -> next;
  }
  if(exists != 1) {
    struct mailBox* newBox = (struct mailBox*) malloc(sizeof(struct mailBox));
    newBox->name = name;
    newBox-> next = NULL;
    newBox-> first = NULL;
    newBox->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    prev->next = newBox;
    send(socket, "OK!", sizeof("OK!"),0);
  }
  else {
    send(socket,"EXIST",sizeof("EXIST"),0);
  }
  return;
}

void DELETE(int socket, char* name) {
  printf("deleting....\n");
  struct mailBox* target;
  int exists = 0;
  struct mailBox* temp = head;
  while(temp->next != NULL) {
    if(strcmp(temp->next->name, name) == 0) {
      target = temp -> next;
      exists = 1;
      break;
    }
    temp = temp-> next;
  }
  if(exists == 1) {
    int lock = pthread_mutex_trylock(&target->mutex);

    if(lock != 0) {
      send(socket, "OPEND", sizeof("OPEND"),0);
      return;
    }
    else {
      pthread_mutex_unlock(&target->mutex);
      if(target->first != NULL) {
        send(socket, "NOTMT", sizeof("NOTMT"), 0);
      }
      else {
        if(target->next == NULL) {
          temp->next = NULL;
          pthread_mutex_destroy(&target->mutex);
          free(target->name);
          free(target);
          send(socket, "OK!", sizeof("OK!"), 0);
        }
        else {
          temp->next = target->next;
          pthread_mutex_destroy(&target->mutex);
          free(target->name);
          free(target);
          send(socket, "OK!", sizeof("OK!"), 0);
        }
      }
    }
  }
  else {
    send(socket, "NEXST", sizeof("NEXST"), 0);
  }
  return;
}
void* threadRequests(void* newSocket) {
  char* finalNext = (char*) malloc(256*sizeof(char));
  int valRead;
  int open = 0;
  int found = 0;
  int new_socket = (int*) newSocket;//gives the thread the socket
  char* nameOpened;
  //printf("%d\n", new_socket);
  char buffer[1024] = {0};
  struct mailBox* current = head;
  valRead = read(new_socket, buffer, 1024);
  printf("%s\n", buffer);
  //HELLO METHOD
  if(strcmp(buffer, "HELLO") == 0) {
    send(new_socket, "HELLO DUMBv0 ready!\n", sizeof("HELLO DUMBv0 ready!\n"),0);
  }
  while(strcmp(buffer, "GDBYE") != 0) {
    found = 0;
    memset(buffer, '\0', sizeof(buffer));
    valRead = read(new_socket,buffer,1024);
    printf("%s\n", buffer);
    if(valRead == 0) {
      free(finalNext);
      return 0;
    }
    //OPEN BOX//////////////////////////////////////////////////////////////
    if(strncmp(buffer,"OPNBX", 5) == 0) {
      printf("opening....\n");
      pthread_mutex_lock(&head->mutex);
      pthread_mutex_unlock(&head->mutex);
      //printf("%s\n", buffer);
      char* target = getName(buffer);
      printf("%s\n", target);
      while(current != NULL) { //gets the correct mailBox we are looking for
        if(strcmp(target, current->name) == 0) {
          if(pthread_mutex_trylock(&current->mutex) == 0) {
            nameOpened = target;
            open = 1;
            found = 1;
            send(new_socket, "OK!", sizeof("OK!"), 0);
            break;
          }
          else {
            found = 1;
            send(new_socket, "USE", sizeof("USE"),0);
            break;
          }
        }
        current = current->next;
      }
      if(found != 1) {
        send(new_socket, "N/A", sizeof("N/A"),0);
      }
    }
    //////////////////////////////////////////////////////////////////////

    //CLOSE BOX//////////////////////////////////////////////////////////
    else if(strncmp(buffer, "CLSBX",5) == 0) {
      printf("closing...\n");
      char* name3 = getName(buffer);
      if((strcmp(name3, nameOpened) != 0) || open == 0) {
        send(new_socket, "NOOPN", sizeof("NOOPN"), 0);
      }
      else {
        send(new_socket, "OK!", sizeof("OK!"),0);
        open = 0;
        pthread_mutex_unlock(&current->mutex);
      }
      memset(name3, '\0', sizeof(name3));
    }
    ////////////////////////////////////////////////////////////////////

    //CREAT////////////////////////////////////////////////////////////
    else if(strncmp(buffer,"CREAT",5) == 0) {
      char* name1 = getName(buffer);
      pthread_mutex_lock(&head->mutex);
      CREATE(new_socket, name1);
      pthread_mutex_unlock(&head->mutex);
      getBoxNames();
    }
    //////////////////////////////////////////////////////////////////

    //DELBX//////////////////////////////////////////////////////////
    else if(strncmp(buffer,"DELBX",5) == 0) {
      printf("deleting...\n");
      char* name2 = getName(buffer);
      pthread_mutex_lock(&head->mutex);
      DELETE(new_socket, name2);
      pthread_mutex_unlock(&head->mutex);
      getBoxNames();
    }
    ////////////////////////////////////////////////////////////////

    else if(open == 1) {
      if(strncmp(buffer, "PUTMG",5) == 0) {
        printf("putting...\n");
        char lengthStr[100];
        int counterLength = 0;
        int p = 6;
        while(buffer[p] != '!') {
          lengthStr[counterLength] = buffer[p];
          p++;
          counterLength++;
        }
        lengthStr[counterLength] = '\0';
        p++;
        int length = atoi(lengthStr);
        int messageCounter = 0;
        char* newMessage = (char*) malloc(length+1 * sizeof(char));
        while(buffer[p] != '\0') {
          newMessage[messageCounter] = buffer[p];
          p++;
          messageCounter++;
        }
        newMessage[messageCounter] = '\0';
        struct messages* newMsg = (struct messages*) malloc(sizeof(struct messages));
        newMsg->msg = newMessage;
        newMsg->next = NULL;
        if(current->first != NULL) {
          struct messages* msgTrav = current->first;
          while(msgTrav->next != NULL) {
            msgTrav = msgTrav->next;
          }
          msgTrav->next = newMsg;
        }
        else {
          current->first = newMsg;
        }
        send(new_socket, "OK!", sizeof("OK!"),0);
      }

      else if(strcmp(buffer,"NXTMG") == 0) {
        if(current->first == NULL) {
          send(new_socket, "EMPTY", sizeof("EMPTY"),0);
        }
        else{
          struct messages* sendingMsg = current->first;
          char* messageTemp = sendingMsg->msg;
          //printf("%s\n", messageTemp);
          int lengthNext = (int) strlen(messageTemp);
          snprintf(finalNext, sizeof(finalNext), "OK!%d!", lengthNext);
          strcat(finalNext, messageTemp);
          current->first = current->first->next;
          free(sendingMsg->msg);
          free(sendingMsg);
          send(new_socket, finalNext, strlen(finalNext), 0);
          memset(finalNext, '\0', sizeof(finalNext));
        }
      }
      else {
        send(new_socket, "ER:WHAT?", sizeof("ER:WHAT?"),0);
      }
    }
    else if((strncmp(buffer, "PUTMG",5) == 0) || (strncmp(buffer, "NXTMG",5) == 0)) {
      printf("error box not open\n");
      send(new_socket, "NOOPN", sizeof("NOOPN"), 0);
    }
    else {
      send(new_socket, "ER:WHAT?", strlen(buffer), 0);
    }
  }
  //free(name2);
  //free(name3);
  //free(name1);
  //free(target);
  free(finalNext);
  return 0;
}
int main (int argc, char** argv) {
  head = (struct mailBox*) malloc(sizeof(struct mailBox));
  char* someName = (char*) malloc(4*sizeof(char));
  someName = "255\0";
  head->name = someName; //holder name that shold be the head, shouuld never be the same
  head->next = NULL;
  head->first = NULL;
  head->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
  //SOCKE STUFF DW ABOUT IT////////////////////////////////////
  int server_fd, newSocket, valRead;
  struct sockaddr_in address;
  int opt = 1;
  int addrLen = sizeof(address);
  char buffer[1024] = {0};
  char* ipAddr = (char*) malloc(30 * sizeof(char));
  if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(atoi(argv[1]));

  if(bind(server_fd, (struct sockaddr*) &address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  if(listen(server_fd, 10) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  /////////////////////////////////////////////////////////////
  while(1) {
    if((newSocket = accept(server_fd, (struct sockaddr*) &address, (socklen_t *) &addrLen)) < 0) {
      perror("accept");
      exit(EXIT_FAILURE);
    }
    inet_ntop(AF_INET, &address, ipAddr, INET_ADDRSTRLEN);
    printf("%s\n", ipAddr);
    printf("ACCEPTING NEW SOCKET\n");
    //printf("outside of thread socket: %d\n", newSocket);
    pthread_t tid;
    pthread_create(&(tid), NULL, threadRequests, (void*) newSocket);
    pthread_detach(tid);
  }
  return 0;
}
