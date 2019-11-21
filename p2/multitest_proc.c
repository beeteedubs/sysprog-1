#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

int search(int* array, int target, int size) {
  struct timeval start;
  struct timeval end;
  suseconds_t timer = 0;
  gettimeofday(&start, 0);
  int found = 0;
  int status;
  int targetIndex = 0;
  int processAmount = (size / 250);
  int remainderNum = (size % 250);
  if(remainderNum != 0) {
    processAmount++;
    //printf("there is a remainder, %d\n", remainderNum);
  }
  //printf("process amount is %d\n", processAmount);
  pid_t* id = (pid_t*) malloc(sizeof(pid_t) * processAmount); // pid of current proccess
  //pid_t id;
  int i = 0;
  for(i = 0; i < processAmount; i++) {
    id[i] = fork();
    if(id[i] == 0 ) {
      break;
    }
  }
  if(i < processAmount) { //Child processes
    int startingIndex = i * 250;
    if(remainderNum != 0 && i == (processAmount - 1)) { //for the last indivisible case
      int counterRem = 0;
      for(counterRem = 0; counterRem < remainderNum; counterRem++) {
        //printf("Checking index %d\n", startingIndex);
        if(array[startingIndex] == target) {
          found = 1;
          exit(counterRem);
        }
        startingIndex++;
      }
      if(found == 0) {
        //printf("Not found as the %dth child\n", i);
        exit(255);
      }
    }
    else {
      int counter = 0;
      for(counter = 0; counter < 250; counter++) {
        if(array[startingIndex] == target) {
          found = 1;
          exit(counter);
        }
        startingIndex++;
      }
      if(found == 0) {
        //printf("Not found as the %dth child\n", i);
        exit(254);
      }
    }
  }
  int counter2 = 0;
  for(counter2 = 0; counter2 < processAmount; counter2++) {
    if((waitpid(id[counter2], &status, 0)) == -1) {
      printf("waitpid() failure\n");
    }
    if( WIFEXITED(status) ) {
      if(WEXITSTATUS(status) != 254) {
        found = 1;
        targetIndex = WEXITSTATUS(status) + (counter2 * 250);
        //printf("Found target index at %dth index\n", targetIndex);
      }
    }
  }
  gettimeofday(&end,0);
  timer = timer + ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec));
  printf("Time alloted is %d\n", timer);
  free(id);
  if(found == 1) {
    return targetIndex;
  }
  else return -1;
}
