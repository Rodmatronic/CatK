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

int sys_chdir(const char * path) {
  struct task * p = get_current_task();
  struct inode * tmp = NULL;
  char * newdir = NULL;
  tmp = namei(path);
  if(!tmp) {
    if(!tmp->u.generic_ino)
      free(tmp);
    return -ENOENT;
  }
  /* we dont need the inode stuff anymore */
  if(!tmp->u.generic_ino)
    free(tmp);
  strncpy(p->cwd, path, PATH_MAX);
  return 0;
}
