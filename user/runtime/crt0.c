#include "unistd.h"

extern int main();

void _start(void) {
  int rc = main();
  _exit(rc);
  __builtin_unreachable();
}
