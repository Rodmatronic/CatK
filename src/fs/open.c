#include <catk/fs.h>
#include <catk/debug.h>
#include <catk/printk.h>
#include <catk/device.h>
#include <catk/errno.h>
#include <catk/mem.h>
#include <lib/common.h>
#include <stdint.h>

struct filesystem * fs = NULL;

int vfs_open(struct file * file, char * path)
{
  int rc;
  fs = get_filesystem("ext2");
  if(!fs)
    return -ENXIO;
  rc = fs->fsops->lookup(file, path);
  if(IS_ERR(rc))
    return rc;
  return 0;
  //return _file->ops->open(file);
}
