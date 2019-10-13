/*
 *Hello, this is the interface/header file that corresponds to the library file
 *Order of things might seem weird but works
 */

// this is the guard
#ifndef _HEADER_H
#define _HEADER_H

// 1st macro replace malloc(x) calls
#define malloc(x) mymalloc(x)
// 2nd macro replace free(x) calls
#define free(x) myfree(x)

// just function signature
void* mymalloc(int x);
void  myfree(void* x);

// the metadata struct, 2 things (6 bytes total)
//      1. int - magic number to indicate is struct
//          - use int rather than short or char cuz less chance user uses
//            magic number by accident
//      2. short - how much space next alloted
struct meta {
    int magicNum;
    short used;
};

#endif
