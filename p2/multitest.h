#ifndef KITTENS_H
#define KITTENS_H

#define search(x,y,z) search(x,y,z)
int search(int* array, int target, int size);

struct thread_vars{
    int* array;
    int target;
    int arrayLen;
};
#endif
