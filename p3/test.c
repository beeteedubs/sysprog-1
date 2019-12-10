#include <stdio.h>
#include <stdlib.h>

struct node {
  int data;
  struct node* next;
};
int main() {
  char* imTired = "255\0";
  char* what = "this is very long ok";
  strcmp(what, imTired);
}
