#include "unistd.h"

extern int main();

void _start() {
  /* setup file descriptors */
  open("/dev/tty", 2, 0); // stdin
  open("/dev/tty", 2, 0); // stdout
  open("/dev/tty", 2, 0); // stderr
  // get args
  int argc = get_argc();
  char **argv;
  get_argv(argv);

  int rc = main(argc, (const char *)argv);
  _exit(rc);
  for(;;);
  __builtin_unreachable();
}
