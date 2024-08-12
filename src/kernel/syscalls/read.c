#include <catk/core.h>
#include <catk/debug.h>
#include <catk/syscall.h>
#include <catk/task.h>
#include <catk/printk.h>
#include <catk/vfs.h>
#include <catk/errno.h>
#include <stdint.h>

int sys_read(int fd, char * buf, size_t sz)
{
  struct task * p = get_current_task();
  struct file * file = p->fd[fd];
  if(!file->ops) {
    return -EBADF;
  }
  return vfs_read(file, (void *)buf, sz);
}
