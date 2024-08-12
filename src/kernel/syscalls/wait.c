#include <catk/core.h>
#include <catk/compiler.h>
#include <catk/debug.h>
#include <catk/syscall.h>
#include <catk/printk.h>
#include <catk/errno.h>
#include <catk/tty.h>
#include <catk/task.h>
#include <lib/common.h>
#include <stdint.h>

pid_t sys_wait(int * status) {
  if(!task_has_children())
    return -ECHILD;
  pid_t pid = sleep();
  *status = get_task_from_pid(pid)->error_code;
  return pid;
}
