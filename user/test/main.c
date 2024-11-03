#include "stdio.h"
#include "unistd.h"

int main() {
  printf("CatK syscall tester\n");
  int rc = getpid();
  printf("sys_getpid: %d\n", rc);
  if(rc != 1) {
    printf("Syscall test failed\n");
    return -1;
  }
  char input[255];
  printf("Please type 'fizzbuzz'\n");
  rc = read(0, input, 255);
  if(strcmp(input, "fizzbuzz") != 0 || rc < 0) {
    printf("Syscall test failed\n");
    return -1;
  }
  printf("CatK syscall test complete.\n");
  return 0;
}
