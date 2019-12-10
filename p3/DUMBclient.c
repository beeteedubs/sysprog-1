#include <stdio.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
int main(int argc, char** argv) {
  int sock = 0;
  int valRead;
  int error = 0;
  char response[1024] = {0};
  struct sockaddr_in serv_addr;
  if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Socket creation error \n");
    exit(EXIT_FAILURE);
  }

  serv_addr.sin_family = AF_INET;
  if(atoi(argv[2]) <= 4000) {
    printf("Error: port number is too low.\n");
    return;
  }
  serv_addr.sin_port = htons(atoi(argv[2]));

  if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0) {
    printf("\nInvalid address/ Address not supported \n");
    return -1;
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    printf("\nConnection Failed \n");
    return -1;
  }
  //HELLO METHOD
  send(sock, "HELLO", sizeof("HELLO"),0);
  valRead = read(sock, response, 1024);
  printf("%s\n", response); //should be Hello welcome to DUMBv0 or something
  if(strcmp(response, "HELLO DUMBv0 ready!\n") != 0) {
    printf("Error: connected to wrong server\n");
    close(sock);
    return 0;
  }
  //INPUTS
  int nextCount = 0;
  int responseCounter = 0;
  int z = 0;
  char* openedBox = (char*) malloc(26 * sizeof(char));
  char* request = (char*) malloc(256*sizeof(char));
  char* temp = (char*) malloc(256 * sizeof(char)); //user input
  char* finalMsg = (char*) malloc(256*sizeof(char));
  char* msgResponse = (char*) malloc(256*sizeof(char));
  char* msgLen = (char*) malloc(100 * sizeof(char));
  while(strcmp(temp, "quit") != 0) {
    memset(response, 0, sizeof(response));
    memset(request, 0, sizeof(request));
    memset(temp, 0, sizeof(temp));
    printf("> ");
    scanf("%[^\n]%*c", temp);
    if(strcmp("quit", temp) == 0) {
      send(sock, "GDBYE", sizeof("GDBYE"),0);
      continue;
    }

    if(strcmp("open", temp) == 0) {
      printf("Okay, which message box?\n");
      memset(request, 0, sizeof(request));
      memset(temp, 0, sizeof(temp));
      memset(finalMsg, '\0', sizeof(finalMsg));
      //fgets(request, sizeof(request), stdin);
      //sscanf(request, "%255[^\n]s", temp); //name of box
      scanf("%[^\n]%*c", temp);
      strcpy(finalMsg, "OPNBX ");
      strcat(finalMsg, temp);
      printf("%s\n", finalMsg);
      send(sock, finalMsg, strlen(finalMsg), 0);
      valRead = read(sock, response, 1024);
      printf("response: %s\n", response);
      if(strcmp(response, "OK!") == 0) {
        strcpy(openedBox, temp);
        printf("Success! Message box %s is now open.\n", openedBox);
      } //still need an iff statement if its in use
      else if(strcmp(response,"N/A") == 0){
        printf("Error. Message box %s does not exist\n", temp);
      }
      else{
        printf("Error. Message box %s is in use\n", temp);
      }
      continue;
    }

    else if(strcmp("close",temp) == 0) {
      printf("Okay, which message box?\n");
      memset(request, 0, sizeof(request));
      memset(temp, 0, sizeof(temp));
      memset(finalMsg, '\0', sizeof(finalMsg));
      //fgets(request, sizeof(request), stdin);
      //sscanf(request, "%255[^\n]s", temp); //name of box
      scanf("%[^\n]%*c", temp);
      strcpy(finalMsg, "CLSBX ");
      strcat(finalMsg, temp);
      printf("%s\n", finalMsg);
      send(sock, finalMsg, strlen(finalMsg), 0);
      valRead = read(sock, response, 1024);
      printf("response: %s\n", response);
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
      memset(request, 0, sizeof(request));
      memset(temp, 0, sizeof(temp));
      memset(finalMsg, '\0', sizeof(finalMsg));
      //fgets(request, sizeof(request), stdin);
      //sscanf(request, "%255[^\n]s", temp);
      scanf("%[^\n]%*c", temp);
      strcpy(finalMsg, "CREAT ");
      strcat(finalMsg, temp);
      printf("%s\n", temp);
      printf("%s\n", finalMsg);
      send(sock, finalMsg, strlen(finalMsg), 0);
      valRead = read(sock, response, 1024);
      printf("response: %s\n", response);
      if(strcmp(response, "OK!") == 0) {
        printf("Success! Message box %s is now created\n", temp);
      }
      else if(strcmp(response, "EXIST") == 0) {
        printf("Error: Message box %s already exists\n", temp);
      }
    }

    else if(strcmp("delete", temp) == 0) {
      printf("Okay, what is the name of the target box?\n");
      memset(request, 0, sizeof(request));
      memset(temp, 0, sizeof(temp));
      memset(finalMsg, '\0', sizeof(finalMsg));
      //fgets(request, sizeof(request), stdin);
      //sscanf(request, "%255[^\n]s", temp); //name of box
      scanf("%[^\n]%*c", temp);
      strcpy(finalMsg, "DELBX ");
      strcat(finalMsg, temp);
      printf("%s\n", finalMsg);
      send(sock, finalMsg, strlen(finalMsg), 0);
      valRead = read(sock, response, 1024);
      printf("response: %s\n", response);
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
      memset(request, 0, sizeof(request));
      memset(temp, 0, sizeof(temp));
      memset(finalMsg, '\0', sizeof(finalMsg));
      //fgets(request, sizeof(request), stdin);
      //sscanf(request, "%255[^\n]s", temp); //the message
      scanf("%[^\n]%*c", temp);
      int length = strlen(temp);
      printf("%d\n", sizeof(finalMsg));
      snprintf(finalMsg, 256, "PUTMG!%d", length);
      strcat(finalMsg, "!");
      strcat(finalMsg, temp);
      printf("%s\n", finalMsg);
      //PUTMG!msgLen!temp
      send(sock, finalMsg, strlen(finalMsg), 0);
      valRead = read(sock, response, 1024);
      printf("response: %s\n", response);
      if(strncmp("OK!", response,3) == 0) {
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
      printf("response: %s\n", response);
      if(strncmp("OK!", response, 3) == 0) {
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
    else {
      printf("Error: Invalid input\n");
    }
    //send(sock, temp, strlen(temp), 0);
    //valRead = read(sock, response, 1024);
  }
  free(request);
  free(temp);
  free(finalMsg);
  close(sock);
  return 0;
}
