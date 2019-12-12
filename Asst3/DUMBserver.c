#include <stdio.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include "DUMBserver.h"
#include <pthread.h>
#include <time.h>
#include<signal.h>

struct mailBox* head;

void signal_handler() {
  //freeing everything
  printf("\nShutting down...\n");
  struct mailBox* temp = head;
  struct mailBox* tempNext;
  struct messages* tempMsg;
  struct messages* msgNext;
  if(temp->next == NULL) {
    pthread_mutex_destroy(&temp->mutex);
    free(temp);
    exit(0);
  }
  else temp = temp->next;
  while(temp != NULL) {
    if(temp->first != NULL) {
      tempMsg = temp->first;
      while(tempMsg != NULL) {
        msgNext = tempMsg->next;
        free(tempMsg->msg);
        free(tempMsg);
        tempMsg = msgNext;
      }
    }
    msgNext = temp->next;
    pthread_mutex_destroy(&temp->mutex);
    free(temp->name);
    free(temp);
    temp = msgNext;
  }
  exit(0);
}
char* getMonth(int numMonth) {
  //getting the Str equivalent to months from the tm struct
  char* nameMonth = (char*) malloc(4*sizeof(char));
  if(numMonth == 1) {
    nameMonth = "Jan";
  }
  else if(numMonth == 2) {
    nameMonth = "Feb";
  }
  else if(numMonth == 3) {
    nameMonth = "Mar";
  }
  else if(numMonth == 4) {
    nameMonth = "Apr";
  }
  else if(numMonth == 5) {
    nameMonth = "May";
  }
  else if(numMonth == 6) {
    nameMonth = "Jun";
  }
  else if(numMonth == 7) {
    nameMonth = "Jul";
  }
  else if(numMonth == 8) {
    nameMonth = "Aug";
  }
  else if(numMonth == 9) {
    nameMonth = "Sept";
  }
  else if(numMonth == 10) {
    nameMonth = "Oct";
  }
  else if(numMonth == 11) {
    nameMonth = "Nov";
  }
  else if(numMonth == 12) {
    nameMonth = "Dec";
  }
  else {
    nameMonth = "Err";
  }
  return nameMonth;
}
/*
void getBoxNames() {
  struct mailBox* temp = head;
  while(temp != NULL) {
    printf("%s\n", temp->name);
    temp = temp->next;
  }
}
*/
char* getName(char* buffer) {
  //gets everything after the first 5 bytes as a char only called for open, close, and create
  //since they have to read in names
  char* target = (char*) malloc(256 * sizeof(char));
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
  return target;
}

void CREATE(int socket, char* name, char hour[], char min[], int day, char* month, char* clientIP) {
  //checks if name of requested box is within parameters
  if(strlen(name) < 5 || strlen(name) > 25) {
    fprintf(stderr,"%s%s %d %s %s ER:WHAT?\n", hour, min, day, month, clientIP);
    send(socket, "ER:WHAT?", sizeof("ER:WHAT?"),0);
    return;
  }
  if(!(isalpha(name[0]))) {
    fprintf(stderr,"%s%s %d %s %s ER:WHAT?\n", hour, min, day, month, clientIP);
    send(socket, "NMBER", sizeof("NMBER"),0);
    return;
  }
  ////////////////////////////////////////////////////
  //if it exists then we just return since it just throws an error and doesn't do anything
  int exists = 0;
  struct mailBox* temp = head;
  struct mailBox* prev;
  while(temp != NULL) {
    if(strcmp(temp->name, name) == 0) {
      exists = 1;
      break;
    }
    prev = temp;
    temp = temp -> next;
  }
  if(exists != 1) {
    //creating a new mailbox just linked list stuff
    printf("%s%s %d %s %s CREAT\n", hour, min, day, month, clientIP);
    struct mailBox* newBox = (struct mailBox*) malloc(sizeof(struct mailBox));
    newBox->name = name;
    newBox-> next = NULL;
    newBox-> first = NULL;
    newBox->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    prev->next = newBox;
    send(socket, "OK!", sizeof("OK!"),0);
  }
  else {
    fprintf(stderr, "%s%s %d %s %s ER:EXIST\n", hour, min, day, month, clientIP);
    send(socket,"EXIST",sizeof("EXIST"),0);
  }
  return;
}

void DELETE(int socket, char* name, char hour[], char min[], int day, char* month, char* clientIP) {
  int val = 0;
  if(strlen(name) == 0) {
    fprintf(stderr, "%s%s %d %s %s ER:NEXST",hour,min,day,month,clientIP);
    send(socket, "NEXST", sizeof("NEXST"), 0);
    return;
  }
  struct mailBox* target;
  int exists = 0;
  struct mailBox* temp = head;
  //checking if the box really exists if not then just throw the error and return
  while(temp->next != NULL) {
    if(strcmp(temp->next->name, name) == 0) {
      target = temp -> next;
      exists = 1;
      break;
    }
    temp = temp-> next;
  }
  if(exists == 1) {
    //seeing if the box is currently opened
    int lock = pthread_mutex_trylock(&target->mutex);

    if(lock != 0) {
      fprintf(stderr, "%s%s %d %s %s ER:OPEND\n", hour, min, day, month, clientIP);
      send(socket, "OPEND", sizeof("OPEND"),0);
      return;
    }
    else {
      pthread_mutex_unlock(&target->mutex);
      if(target->first != NULL) {
        //can only delete empty boxes
        fprintf(stderr, "%s%s %d %s %s ER:NOTMT\n", hour, min, day, month, clientIP);
        send(socket, "NOTMT", sizeof("NOTMT"), 0);
      }
      else {
        //case where deleting the box at the end of the list
        if(target->next == NULL) {
          temp->next = NULL;
          pthread_mutex_destroy(&target->mutex);
          free(target->name);
          free(target);
          printf("%s%s %d %s %s DELBX\n", hour, min, day, month, clientIP);
          send(socket, "OK!", sizeof("OK!"), 0);
        }
        else {
          //deleting box in the middle of the list
          temp->next = target->next;
          pthread_mutex_destroy(&target->mutex);
          free(target->name);
          free(target);
          printf("%s%s %d %s %s DELBX\n", hour, min, day, month, clientIP);
          send(socket, "OK!", sizeof("OK!"), 0);
        }
      }
    }
  }
  else {
    fprintf(stderr, "%s%s %d %s %s ER:NEXST\n", hour, min, day, month, clientIP);
    send(socket, "NEXST", sizeof("NEXST"), 0);
  }
  return;
}
void* threadRequests(void* vars) {
  char* finalNext = (char*) malloc(256*sizeof(char));
  int valRead;
  int open = 0;
  int found = 0;
  //passing in the arguments from the thread
  struct thread_vars* args = (struct thread_vars*) vars;
  int new_socket = args->socket;//gives the thread the socket
  char* clientIP = args->IP;
  char* nameOpened = (char*) malloc(256*sizeof(char));
  //time stuff
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  int hour1 = tm.tm_hour;
  int min1 = tm.tm_min;
  int day = tm.tm_mday;
  int numMonth = tm.tm_mon + 1;
  char* month = getMonth(numMonth);
  char buffer[1024] = {0};
  struct mailBox* current = head;
  valRead = read(new_socket, buffer, 1024);
  char hour[] = "00";
  char min[] = "00";
  hour[0] = hour1/10 + '0';
  hour[1] = hour1%10 + '0';
  min[0] = min1/10 + '0';
  min[1] = min1%10 + '0';
  ////////////
  //HELLO METHOD
  if(strcmp(buffer, "HELLO") == 0) {
    printf("%s%s %d %s %s HELLO\n", hour, min, day, month, clientIP);
    printf("%s%s %d %s %s connected\n", hour, min,day, month, clientIP);
    send(new_socket, "HELLO DUMBv0 ready!\n", sizeof("HELLO DUMBv0 ready!\n"),0);
  }
  else{
    strcpy(buffer, "GDBYE");
  }
  //////////////
  while(strcmp(buffer, "GDBYE") != 0) {
    memset(buffer, '\0', sizeof(buffer));
    valRead = read(new_socket,buffer,1024);
    found = 0;
    t = time(NULL);
    tm = *localtime(&t);
    hour1 = tm.tm_hour;
    min1 = tm.tm_min;
    day = tm.tm_mday;
    numMonth = tm.tm_mon + 1;
    month = getMonth(numMonth);
    hour[0] = hour1/10 + '0';
    hour[1] = hour1%10 + '0';
    min[0] = min1/10 + '0';
    min[1] = min1%10 + '0';
    if(valRead == 0) {
      printf("%s%s %d %s %s GDBYE\n", hour, min, day, month, clientIP);
      printf("%s%s %d %s %s disconnected\n", hour, min,day, month, clientIP);
      break;
    }
    //OPEN BOX//////////////////////////////////////////////////////////////
    if(strncmp(buffer,"OPNBX ", 6) == 0) {
      pthread_mutex_lock(&head->mutex);
      pthread_mutex_unlock(&head->mutex);
      found = 0;
      char* target = getName(buffer);
      //printf("%s\n", target);
      if(strlen(target) == 0) {
	       fprintf(stderr, "%s%s %d %s %s ER:NEXST", hour,min, day,month,clientIP);
	       send(new_socket, "N/A", sizeof("N/A"),0);
	       free(target);
	       continue;
      }
      if(strcmp("255", target) == 0) {
        fprintf(stderr, "%s%s %d %s %s ER:NEXST\n", hour, min, day, month, clientIP);
        send(new_socket, "N/A", sizeof("N/A"),0);
        continue;
      }
      if(open == 1) {
        fprintf(stderr,"%s%s %d %s %s ER:ALROP\n", hour, min, day, month, clientIP); //already open
        send(new_socket, "SPECIAL", sizeof("SPECIAL"),0);
        continue;
      }
      current = head;
      while(current != NULL) { //gets the correct mailBox we are looking for
        if(strcmp(target, current->name) == 0) {
          printf("found it\n");
          if(pthread_mutex_trylock(&current->mutex) == 0) {
            nameOpened = target;
            open = 1;
            found = 1;
            printf("%s%s %d %s %s OPNBX\n", hour, min, day, month, clientIP);
            send(new_socket, "OK!", sizeof("OK!"), 0);
            break;
          }
          else {
            found = 1;
            fprintf(stderr, "%s%s %d %s %s ER:OPEND\n", hour, min, day, month, clientIP);
            send(new_socket, "USE", sizeof("USE"),0);
            break;
          }
        }
        current = current->next;
      }
      if(found == 0) {
        fprintf(stderr, "%s%s %d %s %s ER:NEXST\n", hour, min, day, month, clientIP);
        send(new_socket, "N/A", sizeof("N/A"),0);
      }
      free(target);
      continue;
    }
    //////////////////////////////////////////////////////////////////////

    //GOODBYE////////////////////////////////////////////////////////////
    else if(strcmp(buffer, "GDBYE") == 0) {
      printf("%s%s %d %s %s GDBYE\n", hour, min, day, month, clientIP);
      printf("%s%s %d %s %s disconnected\n", hour, min,day, month, clientIP);
      continue;
    }
    ////////////////////////////////////////////////////////////////////

    //CLOSE BOX//////////////////////////////////////////////////////////
    else if(strncmp(buffer, "CLSBX ",6) == 0) {
      char* name3 = getName(buffer);
      if(strlen(name3) == 0) {
	       fprintf(stderr, "%s%s %d %s %s ER:NOOOPN\n",hour,min,day,month,clientIP);
	        send(new_socket, "NOOPN", sizeof("NOOPN"), 0);
	         continue;
      }
      else if((strcmp(name3, nameOpened) != 0) || open == 0) {
        fprintf(stderr, "%s%s %d %s %s ER:NOOPN\n", hour, min, day, month, clientIP);
        send(new_socket, "NOOPN", sizeof("NOOPN"), 0);
      }
      else {
        printf("%s%s %d %s %s CLSBX\n", hour, min, day, month, clientIP);
        memset(name3, '\0', sizeof(name3));
        open = 0;
        pthread_mutex_unlock(&current->mutex);
        current = head;
	      send(new_socket, "OK!", sizeof("OK!"),0);
      }
      free(name3);
    }
    ////////////////////////////////////////////////////////////////////

    //CREAT////////////////////////////////////////////////////////////
    else if(strncmp(buffer,"CREAT ",6) == 0) {
      char* name1 = getName(buffer);
      pthread_mutex_lock(&head->mutex);
      CREATE(new_socket, name1, hour, min, day, month, clientIP);
      pthread_mutex_unlock(&head->mutex);
    }
    //////////////////////////////////////////////////////////////////

    //DELBX//////////////////////////////////////////////////////////
    else if(strncmp(buffer,"DELBX ",6) == 0) {
      char* name2 = getName(buffer);
      pthread_mutex_lock(&head->mutex);
      DELETE(new_socket, name2, hour, min, day, month, clientIP);
      pthread_mutex_unlock(&head->mutex);
    }
    ////////////////////////////////////////////////////////////////

    else if(open == 1) {
      //PUTMG//////////////////////////////////////////////
      if(strncmp(buffer, "PUTMG!",6) == 0) {
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
        printf("%s%s %d %s %s PUTMG\n", hour, min, day, month, clientIP);
        snprintf(finalNext, sizeof(finalNext), "OK!%d", strlen(newMessage));
        send(new_socket, finalNext, strlen(finalNext),0);
      }
      //////////////////////////////////////////////////////

      //NXTMG//////////////////////////////////////////////
      else if(strcmp(buffer,"NXTMG") == 0) {
        if(current->first == NULL) {
          send(new_socket, "EMPTY", sizeof("EMPTY"),0);
        }
        else{
          struct messages* sendingMsg = current->first;
          char* messageTemp = sendingMsg->msg;
          int lengthNext = (int) strlen(messageTemp);
          snprintf(finalNext, sizeof(finalNext), "OK!%d!", lengthNext);
          strcat(finalNext, messageTemp);
          current->first = current->first->next;
          free(sendingMsg->msg);
          free(sendingMsg);
          printf("%s%s %d %s %s NXTMG\n", hour, min, day, month, clientIP);
          send(new_socket, finalNext, strlen(finalNext), 0);
          memset(finalNext, '\0', sizeof(finalNext));
        }
      }
      //////////////////////////////////////////////////////
      else {
        fprintf(stderr, "%s%s %d %s %s ER:WHAT?\n", hour, min, day, month, clientIP);
        send(new_socket, "ER:WHAT?", sizeof("ER:WHAT?"),0);
      }
    }
    //trying to use put and next when not inside a box
    else if((strncmp(buffer, "PUTMG",5) == 0) || (strncmp(buffer, "NXTMG",5) == 0)) {
      fprintf(stderr, "%s%s %d %s %s ER:NOOPN\n", hour, min, day, month, clientIP);
      send(new_socket, "NOOPN", sizeof("NOOPN"), 0);
    }
    /*
    else if(strcmp("print", buffer) == 0) {
      getBoxNames();
      send(new_socket, "OK!", sizeof("OK!"), 0);
    }
    */
    else {
      fprintf(stderr, "%s%s %d %s %s ER:WHAT?\n", hour, min, day, month, clientIP);
      send(new_socket, "ER:WHAT?", strlen(buffer), 0);
    }
  }
  if(open == 1) {
    //edge case where client quits before closing their box
    pthread_mutex_unlock(&current->mutex);
    open = 0;
  }
  //killing the thread
  free(nameOpened);
  free(finalNext);
  return 0;
}
int main (int argc, char** argv) {
  if(argc > 2 || argc < 2) {
    fprintf(stderr,"Error too little/many arguments\n");
    return 0;
  }
  head = (struct mailBox*) malloc(sizeof(struct mailBox));
  head->name = (char*) malloc(4);
  head->name = "255\0"; //named this because mailboxes can't start with number
  head->next = NULL;
  head->first = NULL;
  head->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
  //SOCKE STUFF DW ABOUT IT////////////////////////////////////
  signal(SIGINT, signal_handler);
  int server_fd, newSocket, valRead;
  struct sockaddr_in address;
  int opt = 1;
  int addrLen = sizeof(address);
  char buffer[1024] = {0};
  char* ipAddr = (char*) malloc(30 * sizeof(char));
  struct thread_vars vars;
  if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    fprintf(stderr, "socket failed");
    return -1;
  }

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
    fprintf(stderr, "setsockopt failed");
    return -1;
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(atoi(argv[1]));

  if(atoi(argv[1]) <= 4000) {
    fprintf(stderr,"Error: port number is too low.\n");
    return -1;
  }

  if(bind(server_fd, (struct sockaddr*) &address, sizeof(address)) < 0) {
    fprintf(stderr,"bind failed");
    return -1;
  }

  if(listen(server_fd, 10) < 0) {
    fprintf(stderr,"listen failed");
    return -1;
  }
  /////////////////////////////////////////////////////////////
  while(1) {
    if((newSocket = accept(server_fd, (struct sockaddr*) &address, (socklen_t *) &addrLen)) < 0) {
      fprintf(stderr,"accept failed");
      return -1;
    }
    inet_ntop(AF_INET, &address.sin_addr, ipAddr, INET_ADDRSTRLEN);
    vars.IP = ipAddr;
    vars.socket = newSocket;
    pthread_t tid;
    pthread_create(&(tid), NULL, threadRequests, (void*) &vars);
    pthread_detach(tid);
  }
  return 0;
}
