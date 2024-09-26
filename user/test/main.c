#include "stdio.h"
#include "unistd.h"

int main(int argc, const char * argv[]) {
  printf("argc: %d, argv: %s\n", argc, argv[0]);
  return 0;
}
