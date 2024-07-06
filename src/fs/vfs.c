#include <catk/fs.h>
#include <catk/debug.h>
#include <catk/printk.h>
#include <catk/device.h>
#include <catk/errno.h>
#include <catk/mem.h>
#include <catk/task.h>
#include <lib/common.h>
#include <stdint.h>

struct filesystem * fs = NULL;

int vfs_open(struct file * filp, const char * file)
{
  debug("[vfs] opening file %s\n", file);
  int rc;
  struct task * p = get_current_task();
  rc = fs->fops->open(filp, file);
  if(IS_ERR(rc))
    return rc;
  p->fd[2] = filp; /* we'll put it at 2 since 0 and 1 are file descriptors for the TTY */
  return 0;
}

int vfs_read(struct file * filp, void * buf, size_t sz) /* soon, i'll change this to (int fd, void * buf, size_t sz) */
{
  debug("[vfs] reading file %s\n", filp->name);
  return fs->fops->read(filp, buf, sz);
}

int vfs_init(void)
{
  fs = get_filesystem("ext2");
  if(!fs)
    return -ENXIO;
  return 0;
}
