#include <catk/core.h>
#include <catk/debug.h>
#include <catk/syscall.h>
#include <catk/task.h>
#include <catk/printk.h>
#include <catk/vfs.h>
#include <catk/errno.h>
#include <stdint.h>

int sys_fork(void) {
  /*
  uint32_t eip = get_eip();
  struct task * p = create_user_task(get_current_task()->name, eip, TASK_PRIORITY_NORMAL);
  p->ppid = get_current_task()->pid;
  if(!p) {
    return -1;
  }
  task_add_queue(p);
  */
  return 0;
}
