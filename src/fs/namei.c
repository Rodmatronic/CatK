#include <catk/fs.h>
#include <catk/vfs.h>
#include <catk/debug.h>
#include <catk/printk.h>
#include <catk/errno.h>
#include <lib/common.h>
#include <stdint.h>

struct inode * namei(const char * pathname) {
  if(strlen(pathname) > PATH_MAX) {
    return NULL;
  }
  struct inode * inode = NULL;
  if(is_devfs(pathname)) {
    inode = get_filesystem("devfs")->fsops->namei(pathname);
  } else {
    inode = vfs_get_rootfs()->fsops->namei(pathname);
  }
  return inode;
}
