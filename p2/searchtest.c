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
  //printf("This is an ordered array: ");
  for(counter = oldSize; counter < newLen; counter += 1){
    *(array + counter) = counter;
    //printf("%d, ", *(array + counter));
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
  //Print the scrambled array
  //printArray(array,arrayLen);
  //printf("\n");

}
void* rescramble(int prevTarget, int* array, int arrayLen){
  int randoNum = rand()%arrayLen;
  //printf("Target value is at index %d\n", randoNum);
  //printf("Swapping indexes %d and %d\n", prevTarget, randoNum);
  int temp = *(array + prevTarget);
  *(array + prevTarget) = *(array + randoNum);
  *(array + randoNum) = temp;
}

int main (int argc, char** argv){
  srand(time(0));
  // VARIABLES
  int returnedIndex;
  int oldSize = 0;
  int counter = 0;
  int arrLen;
  int* arrPtr =(int*) malloc(sizeof(int)* 1000);

  // FILLING IN "arr"
  fillArr(arrPtr, 1000, oldSize);

  // SCRAMBLE
  scramble(arrPtr, 1000);
  int tempCounter = 0;
  for(tempCounter = 0; tempCounter < 1000; tempCounter++) {
    if(arrPtr[tempCounter] == 50) {
      //printf("Target value is at index %d\n", tempCounter);
    }
  }
  returnedIndex = search(arrPtr, 50, 1000);
  //printf("returnedIndex is: %d\n", returnedIndex);
  int reallocSize = 1000; //current size after hard coded 10
  for(counter = 0; counter < 76; counter += 1){
    printf("%d plot point\n", counter);
    printf("\n");
    oldSize = reallocSize;
    reallocSize += 250;
    arrPtr  = (int*) realloc(arrPtr, sizeof(int) * reallocSize);
    fillArr(arrPtr, reallocSize, oldSize);
    rescramble(returnedIndex, arrPtr, reallocSize);
    returnedIndex = search(arrPtr, 50, reallocSize);
    //printf("returnedIndex is: %d\n", returnedIndex);
    if(returnedIndex == -1) {
      printf("error not in the array\n");
      break;
    }
  }
  free(arrPtr);
  return 0;
}
