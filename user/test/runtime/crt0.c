#include "unistd.h"

extern int main();

void _start() {
  int rc = main();
  _exit(rc);
  for(;;);
  __builtin_unreachable();
}
