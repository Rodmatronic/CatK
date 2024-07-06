#include <catk/core.h>
#include <catk/debug.h>
#include <catk/syscall.h>
#include <catk/task.h>
#include <catk/printk.h>
#include <stdint.h>

void sys_exit(int err_code)
{
  debug("[sys_ext] exiting with code: %d\n", err_code);
  kill(get_current_task());
  for(;;);
}
