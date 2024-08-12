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
  inode = get_filesystem(is_devfs(pathname) ? "devfs" : "ext2")->fsops->namei(pathname);
  return inode;
}
