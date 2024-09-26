#include "unistd.h"

extern int main();

void _start(void) {
  open("/dev/tty", 2, 0); // stdin
  open("/dev/tty", 2, 0); // stdout
  open("/dev/tty", 2, 0); // stderr
  int rc = main();
  _exit(rc);
  __builtin_unreachable();
}
