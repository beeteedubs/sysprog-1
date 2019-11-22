#include<stdio.h>
#include<signal.h>
#include<errno.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<signal.h>
#include<string.h>
#include"multitest.h"


//PRINTS ARRAY "array" OF LENGTH "length"
void printArray(int*array, int length){
  int counter = 0;
  while(counter<length){
    printf("%d, ", *(array + counter));
    counter += 1;
  }
}


//CREATES AN ORDERED ARRAY OF LENGTH "newLen"
void* fillArr(int* array, int newLen, int oldSize){
  int counter;
  for(counter = oldSize; counter < newLen; counter += 1){
    *(array + counter) = counter;
  }
}


//SCRAMBLES ARRAY "array"
void* scramble(int*array, int arrayLen){

  int counter2, randoNum;
  int temp = 0;

  //printf("These is a scrambled array: ");
  for(counter2 = 0; counter2 < arrayLen; counter2+=1){

    //CREATE RANDOM INDEX VALUE TO SWAP
    randoNum = rand()%arrayLen; //randomNum = [0,arrayLen -1]

    //Swap values
    temp = *(array + counter2);
    *(array + counter2) = *(array + randoNum);
    *(array + randoNum) = temp;

  }

}
void* rescramble(int prevTarget, int* array, int arrayLen){
  int randoNum = rand()%arrayLen;
  printf("Target value is at index %d\n", randoNum);
  int temp = *(array + prevTarget);
  *(array + prevTarget) = *(array + randoNum);
  *(array + randoNum) = temp;
}

int main (int argc, char** argv){
  srand(time(0));
  // VARIABLES
  int blockSize = 250;
  int returnedIndex;
  int oldSize = 0;
  int arrLen;
  int* arrPtr =(int*) malloc(sizeof(int)* 1000);
  int reallocSize = 250;
  // FILLING IN "arr"
  fillArr(arrPtr, reallocSize, oldSize);

  // SCRAMBLE
  scramble(arrPtr, blockSize);
  int tempCounter = 0;
  for(tempCounter = 0; tempCounter < blockSize; tempCounter++) {
    if(arrPtr[tempCounter] == 50) {
      printf("Target value is at index %d\n", tempCounter);
    }
  }
  returnedIndex = search(arrPtr, 50, reallocSize);
  //printf("Size of array is %d\n", reallocSize); //for test case dealing with array length
  printf("returnedIndex is: %d\n", returnedIndex);
  printf("\n");
  while(reallocSize < 10000){ //searching up until desired array length
    oldSize = reallocSize;
    reallocSize += blockSize;
    arrPtr  = (int*) realloc(arrPtr, sizeof(int) * reallocSize);
    fillArr(arrPtr, reallocSize, oldSize);
    rescramble(returnedIndex, arrPtr, reallocSize);
    returnedIndex = search(arrPtr, 50, reallocSize);
    printf("returnedIndex is: %d\n", returnedIndex);
    //printf("Size of array is %d\n", reallocSize); //for test case dealing with array length
    printf("\n");
    if(returnedIndex == -1) {
      printf("error not in the array\n");
      break;
    }
  }
  //Test Case Edge Case at the very end of the graph
  oldSize = reallocSize;
  reallocSize = 20000;
  arrPtr = (int*) realloc(arrPtr, sizeof(int) * reallocSize);
  fillArr(arrPtr, reallocSize, oldSize);
  rescramble(returnedIndex, arrPtr, reallocSize);
  returnedIndex = search(arrPtr, 50, reallocSize);
  printf("returnedIndex is: %d\n", returnedIndex);
  // printf("Size of array is %d\n", reallocSize);
  printf("\n");
  free(arrPtr);
  return 0;
}
