#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

int search(int* array, int target, int size) {
  printf("Searching with multiprocesses...\n");
  struct timeval start;
  struct timeval end;
  suseconds_t timer = 0;
  gettimeofday(&start, 0);
  int blockSize = 250; //how much each process is splitting
  int found = 0;
  int status;
  int targetIndex = 0; //index we are returning
  int processAmount = (size / blockSize); //how many children processes wanted
  int remainderNum = (size % blockSize); //checking remainder
  if(remainderNum != 0) {
    processAmount++; //adds a process when there is a remainder, remainder is always last child
  }
  //printf("process amount is %d\n", processAmount); //for test case comparing amount of processes
  pid_t* id = (pid_t*) malloc(sizeof(pid_t) * processAmount); // pid of current proccess
  int i = 0;
  for(i = 0; i < processAmount; i++) {
    //creating child procceses
    id[i] = fork();
    if(id[i] == 0 ) {
      //if you are a child process you break
      break;
    }
  }
  if(i < processAmount) {
    //Child processes only step into here
    int startingIndex = i * blockSize;
    if(remainderNum != 0 && i == (processAmount - 1)) {
      //for remainder case
      int counterRem = 0;
      for(counterRem = 0; counterRem < remainderNum; counterRem++) {
        if(array[startingIndex] == target) {
          //if the target is found
          found = 1;
          exit(counterRem);
        }
        startingIndex++;
      }
      if(found == 0) {
        exit(254); //value showing that it is not found
      }
    }
    else {
      int counter = 0;
      for(counter = 0; counter < blockSize; counter++) {
        if(array[startingIndex] == target) {
          //if the target is found
          found = 1;
          exit(counter);
        }
        startingIndex++;
      }
      if(found == 0) {
        exit(254); //value showing it is not found
      }
    }
  }
  //PARENT PROCESSES
  int counter2 = 0;
  for(counter2 = 0; counter2 < processAmount; counter2++) {
    if((waitpid(id[counter2], &status, 0)) == -1) { //Getting exit statuses
      printf("waitpid() failure\n");
    }
    if( WIFEXITED(status) ) {
      if(WEXITSTATUS(status) != 254) {
        //only one exit status should not be 254 if the value is found
        found = 1;
        targetIndex = WEXITSTATUS(status) + (counter2 * blockSize);
        //since exit status can only go up to 250, we return a value ranging from [0, 249]
        //and adding the ith child it is multiplied with blocksize giving the targetIndex in the array
      }
    }
  }
  free(id);
  gettimeofday(&end,0);
  timer = ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec));
  printf("Time alloted is %d\n", timer);
  if(found == 1) {
    return targetIndex;
  }
  else return -1;
}
