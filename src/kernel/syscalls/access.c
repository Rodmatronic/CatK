#include <catk/core.h>
#include <catk/debug.h>
#include <catk/syscall.h>
#include <catk/task.h>
#include <catk/printk.h>
#include <catk/vfs.h>
#include <catk/errno.h>
#include <catk/stat.h>
#include <catk/mem.h>
#include <stdint.h>

int sys_access(const char * path, int mode) {
  if (!(mode & S_IRWXO))
    return -EINVAL;
  struct task * p = get_current_task();
  /* use namei to get the inode of the pathname */
  struct inode * i = namei(path);
  if(p->uid == i->uid) {
    /* devfs uses the generic inode pointer. to prevent accidentally freeing a devfs inode, we do this */
    if(!i->u.generic_ino)
      free(i);
    return 0;
  }
  if(!i->u.generic_ino)
    free(i);
  return -EACCES;
}
