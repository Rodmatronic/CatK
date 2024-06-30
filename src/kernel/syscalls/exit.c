#include <catk/core.h>
#include <catk/debug.h>
#include <catk/syscall.h>
#include <catk/task.h>
#include <stdint.h>

void sys_exit(int err_code)
{
  pid_t pid = get_current_task()->pid;
  kill(pid);
}
