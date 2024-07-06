#include <catk/core.h>
#include <catk/debug.h>
#include <catk/syscall.h>
#include <catk/task.h>
#include <catk/printk.h>
#include <catk/vfs.h>
#include <stdint.h>

int sys_read(int fd, char * buf, size_t sz)
{
  struct task * p = get_current_task();
  return vfs_read((struct file *)p->fd[fd], buf, sz);
}
