#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
void* fillArr(int* array, int newLen){
    int counter;
    for(counter = 0; counter<newLen; counter += 1){
        *(array + counter) = counter;
    }
}

int search(int* array, int target, int size) {
  int found = 0;
  int status;
  int targetIndex = 0;
  int childNumber = 0;
  int processAmount = (size / 250);
  int remainderNum = (size % 250);
  if(remainderNum != 0) {
    processAmount++;
    printf("there is a remainder, %d\n", remainderNum);
  }
  printf("process amount is %d\n", processAmount);
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
    if(remainderNum != 0 && i == (processAmount - 1)) {
      int counterRem = 0;
      for(counterRem = 0; counterRem < remainderNum; counterRem++) {
        printf("Checking index %d\n", startingIndex);
        if(array[startingIndex] == target) {
          found = 1;
          exit(counterRem);
        }
        startingIndex++;
      }
      if(found == 0) {
        printf("Not found as the %dth child\n", i);
        exit(255);
      }
    }
    else {
      int counter = 0;
      for(counter = 0; counter < 250; counter++) {
        if(array[startingIndex] == target) {
          //childNumber = i;
          found = 1;
          exit(counter);
        }
        startingIndex++;
      }
      if(found == 0) {
        printf("Not found as the %dth child\n", i);
        exit(255);
      }
    }
  }
  int counter2 = 0;
  for(counter2 = 0; counter2 < processAmount; counter2++) {
    if((waitpid(id[counter2], &status, 0)) == -1) {
      printf("waitpid() failure\n");
    }
    if( WIFEXITED(status) ) {
      if(WEXITSTATUS(status) != 255) {
        found = 1;
        targetIndex = WEXITSTATUS(status) + (counter2 * 250);
        //printf("Found target index at %dth index\n", targetIndex);
      }
    }
  }
  if(found == 1) {
    return targetIndex;
  }
  else return -1;
}
int main() {
  int* arr = (int*) malloc(sizeof(int) * 550);
  fillArr(arr, 550);
  int z = search(arr, 5255, 550);
  if(z != -1) {
    printf("Success\n");
    printf("Found index is %d\n", z);
  }
  if(z == -1) {
    printf("Failure\n");
  }
  return 0;
}
