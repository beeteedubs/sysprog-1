#include <stdio.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <netdb.h>

void deleteEnter (char* name) {
  int i = 0;
  while(name[i] != '\n') {
    i++;
  }
  name[i] = '\0';
}

int main(int argc, char** argv) {
  if(argc < 3 || argc > 3) {
    fprintf(stderr,"Error too little/many arguments\n");
    return 0;
  }
  int usingHost = 0;
  int sock = 0;
  char* addr;
  char* ip = (char*) malloc(256);
  int valRead;
  struct in_addr **addr_list;
  int error = 0;
  char response[1024] = {0};
  struct sockaddr_in serv_addr;
  addr = strdup(argv[1]);
  if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr,"Socket creation error \n");
    return -1;
  }

  serv_addr.sin_family = AF_INET;
  if(atoi(argv[2]) <= 4000) {
    fprintf(stderr,"Error: port number is too low.\n");
    return -1;
  }
  serv_addr.sin_port = htons(atoi(argv[2]));
  if(isalpha(argv[1][0]) == 0) {
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0) {
      fprintf(stderr,"Invalid address/ Address not supported \n");
      return -1;
    }
  }
  else {
    usingHost = 1;
    struct hostent* host = gethostbyname(addr);
    if(!host) {
        fprintf(stderr, "Host name failed to resolve IP address\n");
        return -1;
    }
    //bcopy((char*) host->h_addr, (char*) &serv_addr.sin_addr, host->h_length);
    memcpy(&serv_addr.sin_addr, host->h_addr_list[0], host->h_length);
  }
  int connectCounter = 1;
  while(connectCounter < 4) {
    //MESSAGE FOR TAs OR WHOEVER IS GRADING THIS
    //if it seems like it is stuck in an infinite look wait a bit, it might just be taking awhile for connect to run
    //that was the case for random ip addresses and www.whitehouse.gov
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == 0) {
      break;
    }
    printf("Connection attempt %d...\n", connectCounter);
    connectCounter++;
  }
  if(connectCounter == 4) {
    fprintf(stderr,"Connection Failed \n");
    return -1;
  }
  //HELLO METHOD
  send(sock, "HELLO", sizeof("HELLO"),0);
  valRead = read(sock, response, 1024);
  //printf("%s\n", response); //should be Hello welcome to DUMBv0 or something
  if(strcmp(response, "HELLO DUMBv0 ready!\n") != 0) {
    fprintf(stderr, "Error: connected to wrong server\n");
    close(sock);
    return 0;
  }
  printf("%s\n", response); //Welcomes the client into the server just keep it
  //INPUTS
  int nextCount = 0;
  int responseCounter = 0;
  int z = 0;
  char* openedBox = (char*) malloc(26 * sizeof(char));
  char* finalMsg = (char*) malloc(256*sizeof(char));
  char* msgResponse = (char*) malloc(256*sizeof(char));
  size_t buffSize = 256;
  char* temp = (char*) malloc(buffSize * sizeof(char));
  while(strcmp(temp, "quit") != 0) {
    memset(temp,0, sizeof(temp));
    memset(response, 0, sizeof(response));
    printf("> ");
    getline(&temp, &buffSize, stdin);
    deleteEnter(temp);
    if(strcmp("quit", temp) == 0) {
      send(sock, "GDBYE", sizeof("GDBYE"),0);
      continue;
    }
    else if(strcmp("help", temp) == 0) {
      printf("quit\t\t(which causes: E.1 GDBYE)\n");
      printf("create\t\t(which causes: E.2 CREAT)\n");
      printf("delete\t\t(which causes: E.6 DELBX)\n");
      printf("open\t\t(which causes: E.3 OPNBX)\n");
      printf("close\t\t(which causes: E.7 CLSBX)\n");
      printf("next\t\t(which causes: E.4 NXTMG)\n");
      printf("put\t\t(which causes: E.5 PUTMG)\n");
    }
    else if(strcmp("open", temp) == 0) {
      printf("Okay, which message box?\n");
      memset(temp, 0, sizeof(temp));
      memset(finalMsg, '\0', sizeof(finalMsg));
      printf("open:> ");
      getline(&temp, &buffSize, stdin);
      deleteEnter(temp);
      strcpy(finalMsg, "OPNBX ");
      strcat(finalMsg, temp);
      //printf("%s\n", finalMsg);
      send(sock, finalMsg, strlen(finalMsg), 0);
      printf("finalMsg: %s\n", finalMsg);
      valRead = read(sock, response, 1024);
      //printf("response: %s\n", response);
      if(strcmp(response, "OK!") == 0) {
        strcpy(openedBox, temp);
        printf("Success! Message box %s is now open.\n", openedBox);
      } //still need an iff statement if its in use
      else if(strcmp(response,"N/A") == 0){
        printf("Error. Message box %s does not exist\n", temp);
      }
      else if(strcmp(response, "SPECIAL") == 0) {
        printf("Error. You currently have a box open, cannot open another\n");
      }
      else{
        printf("Error. Message box %s is in use\n", temp);
      }
      continue;
    }

    else if(strcmp("close",temp) == 0) {
      printf("Okay, which message box?\n");
      memset(finalMsg, '\0', sizeof(finalMsg));
      printf("close:> ");
      getline(&temp, &buffSize, stdin);
      deleteEnter(temp);
      strcpy(finalMsg, "CLSBX ");
      strcat(finalMsg, temp);
      //printf("%s\n", finalMsg);
      send(sock, finalMsg, strlen(finalMsg), 0);
      //printf("finalMsg: %s\n", finalMsg);
      valRead = read(sock, response, 1024);
      //printf("response: %s\n", response);
      if(strcmp(response, "OK!") == 0) {
        printf("Success! Message box %s is now closed.\n", openedBox);
        memset(openedBox, '\0', sizeof(openedBox));
      }
      else {
        if(strcmp(response, "NOOPN") == 0) {
          printf("Error: You do not have that box opened\n");
        }
      }
    }

    else if(strcmp("create", temp) == 0) {
      printf("Okay, what is the name of the new box?\n");
      memset(finalMsg, '\0', sizeof(finalMsg));
      printf("create:> ");
      getline(&temp, &buffSize, stdin);
      deleteEnter(temp);
      strcpy(finalMsg, "CREAT ");
      strcat(finalMsg, temp);
      //printf("%s\n", temp);
      //printf("%s\n", finalMsg);
      send(sock, finalMsg, strlen(finalMsg), 0);
      valRead = read(sock, response, 1024);
      //printf("response: %s\n", response);
      if(strcmp(response, "OK!") == 0) {
        printf("Success! Message box %s is now created\n", temp);
      }
      else if(strcmp(response, "EXIST") == 0) {
        printf("Error: Message box %s already exists\n", temp);
      }
      else if(strcmp(response, "NMBER") == 0) {
        printf("Error: Message boxes cannot start with a number\n");
      }
      else if(strcmp(response, "ER:WHAT?") == 0) {
        printf("Error: Message box names must be at least 5 characters and no more than 25\n");
      }
    }

    else if(strcmp("delete", temp) == 0) {
      printf("Okay, what is the name of the target box?\n");
      memset(finalMsg, '\0', sizeof(finalMsg));
      printf("delete:> ");
      getline(&temp, &buffSize, stdin);
      deleteEnter(temp);
      strcpy(finalMsg, "DELBX ");
      strcat(finalMsg, temp);
      //printf("%s\n", finalMsg);
      send(sock, finalMsg, strlen(finalMsg), 0);
      valRead = read(sock, response, 1024);
      //printf("response: %s\n", response);
      if(strcmp("OK!", response) == 0) {
        printf("Success! Message box %s is now deleted\n", temp);
      }
      else if(strcmp("NEXST", response) == 0) {
        printf("Error: Message box %s does not exist\n", temp);
      }
      else if(strcmp("OPEND", response) == 0) {
        printf("Error: Message box %s is currently opened\n", temp);
      }
      else if(strcmp("NOTMT", response) == 0) {
        printf("Error: Message box %s still has messages\n", temp);
      }
    }

    else if(strcmp("put", temp) == 0) {
      printf("Please enter the message\n");
      memset(finalMsg, '\0', sizeof(finalMsg));
      printf("put:> ");
      getline(&temp, &buffSize, stdin);
      deleteEnter(temp);
      int length = strlen(temp);
      snprintf(finalMsg, 256, "PUTMG!%d", length);
      strcat(finalMsg, "!");
      strcat(finalMsg, temp);
      //PUTMG!msgLen!temp
      send(sock, finalMsg, strlen(finalMsg), 0);
      valRead = read(sock, response, 1024);
      //printf("response: %s\n", response);
      if(strncmp("OK!", response,3) == 0) {
        //printf("%s\n", response);
        printf("Success! Message box %s now has the message: %s\n", openedBox, temp);
      }
      else if(strcmp("NOOPN", response) == 0) {
        printf("Error: You do not have a box opened\n");
      }
    }

    else if(strcmp("next", temp) == 0) {
      strcpy(finalMsg, "NXTMG");
      send(sock, finalMsg, sizeof(finalMsg), 0);
      valRead = read(sock, response, 1024);
      //printf("response: %s\n", response);
      if(strncmp("OK!", response, 3) == 0) {
        memset(msgResponse, '\0', sizeof(msgResponse));
        nextCount = 0;
        z = 0;
        while(nextCount != 2) {
          if(response[z] == '!') {
            nextCount++;
          }
          z++;
        }
        responseCounter = 0;
        while(response[z] != '\0') {
          msgResponse[responseCounter] = response[z];
          z++;
          responseCounter++;
        }
        printf("The next message is: %s\n", msgResponse);
      }
      else if(strcmp("EMPTY", response) == 0) {
        printf("Error: Message box %s is empty\n", openedBox);
      }
      else if(strcmp("NOOPN", response) == 0) {
        printf("Error: You do not have a box opened\n");
      }
    }
    else if(strcmp("print", temp) == 0) {
      send(sock, "print", sizeof("print"),0);
      valRead = read(sock, response, 1024);
    }
    else if(strcmp("printMsg", temp) == 0) {
      send(sock,"printMsg", sizeof("printMsg"),0);
      valRead = read(sock, response, 1024);
    }
    else {
      fprintf(stderr, "Error: Invalid input, for correct commands please type 'help'\n");
    }
  }
  free(openedBox);
  free(msgResponse);
  free(temp);
  free(finalMsg);
  close(sock);
  return 0;
}
