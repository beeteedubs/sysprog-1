#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"
/*
 * make corresponding library
 *  preprocessing, compilation,
 *
 * ? y not include header file in lib
 * ? makefile adding that assign1?
 *
 * metadata struct magic int, cuz more bytes more guaranteed be unique
 * besides magic num, short indicating how long
 *
 * first iteration, scan through enough sizeOf(userData)
 *
 *
 */

//x is # of bytes
//one given, scan array
//how scan?
//  start at beginning of array, check for magic number
//      if magic number, that means metadata so skip ahead appropriately and check progress
//      else scan next x bytes checking for magic number
//          if can't scan x bytes by hitting magic number, skip ahead appropriately
//          else create the metadata and throw in user data
//skip ahead appropriately:
//  traverse until see the short "use", and then jump ahead that much + 2
//throw in user data:
//  returning a pointer should be a memory address of arr


int main(){
    /*
    arr[6] = 'a';
    arr[7] = 'b';
    arr[8] = 'c';
    arr[9] = '\0';
    */

    char *ptr = (char*) malloc(3000);
    char * ptr2 = (char*) malloc(1070);
    char * ptr3 = (char*) malloc(30);
    printf("This is ptr's val: %p\n", ptr);
    printf("This is ptr2's val: %p\n", ptr2);
    printf("This is ptr3's val: %p\n", ptr3);
    free(ptr);
    /*
    int i;
    for (i = 0; i<=3; i++) {
        printf("These are the values of arr at index %d: %c\n", i, *(ptr + i));
    }
    printf("This is ptr's val: %c\n", *ptr);
    */

    return 1;
}
