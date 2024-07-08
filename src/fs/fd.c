#include <catk/fs.h>
#include <catk/debug.h>
#include <catk/printk.h>
#include <catk/device.h>
#include <catk/errno.h>
#include <catk/mem.h>
#include <catk/task.h>
#include <catk/limits.h>
#include <lib/common.h>
#include <stdint.h>

/* finds a free file descriptor for a process */
int file_desc_alloc(void)
{
  struct task * p = get_current_task();
  for(int i = 0; i < OPEN_MAX; i++)
  {
    if(!p->fd[i])
      return i;
  }
  return -ENFILE;
}

/* releases a file descriptor from a process */
void file_desc_free(int fd)
{
  struct task * p = get_current_task();
  p->fd[fd] = NULL;
}
