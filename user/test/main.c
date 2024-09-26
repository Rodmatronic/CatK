#include "stdio.h"
#include "unistd.h"

int main() {
  int rc;
  char buffer[16];
  printf("CatK syscall test.\n");
  rc = open("/COPYRIGHT", 2, 0);
  if(rc < 0) {
    printf("bye\n");
    return 1;
  }
  read(rc, buffer, 16);
  printf("%s", buffer);
  return 0;
}
