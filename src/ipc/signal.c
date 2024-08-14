#include <catk/errno.h>
#include <catk/debug.h>
#include <catk/task.h>
#include <catk/ipc.h>

int dispatch_signal(int signal) {
  /* this is basically just a wrapper function to make people's lives easier. :P */
  return get_current_task()->handle_signal(signal);
}
