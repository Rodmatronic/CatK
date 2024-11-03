#include <catk/core.h>
#include <catk/debug.h>
#include <catk/syscall.h>
#include <catk/task.h>
#include <catk/printk.h>
#include <stdint.h>

void sys_exit(int err_code)
{
  struct task * p = get_current_task();
  debug("sys_exit: pid %d exiting with code: %d\n", p->pid, err_code);
  p->error_code = err_code;
  kill(p);
  for(;;);
}
