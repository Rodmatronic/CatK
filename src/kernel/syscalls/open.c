#include <catk/debug.h>
#include <catk/syscall.h>
#include <catk/printk.h>
#include <catk/errno.h>
#include <catk/vfs.h>
#include <catk/task.h>
#include <catk/mem.h>
#include <lib/common.h>
#include <stdint.h>

/* flags and mode are unused */
int sys_open(const char * filename, int flags, uint16_t mode)
{
  struct task * p = get_current_task();
  int rc = 0;
  int fd = file_desc_alloc();
  /* 
   * i would have used IS_ERR, but it would detect this wrong
   * since the return code could be 1-32.
  */
  if(fd < 0)
  {
    debug("sys_open: uh oh: %d\n", fd);
    /* fd contains the error code */
    return fd;
  }
  /* allocate vfs file */
  struct file * filp = (struct file *)malloc(sizeof(struct file));
  /* ^-- gets deallocated when sys_close is called */
  rc = vfs_open(filp, filename);
  if(IS_ERR(rc))
  {
    debug("sys_open: oh no! %d\n", rc);
    return rc;
  }
  p->fd[fd] = filp;
  return fd;
}
