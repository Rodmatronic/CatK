#include <catk/fs.h>
#include <catk/debug.h>
#include <catk/printk.h>
#include <catk/device.h>
#include <catk/errno.h>
#include <catk/mem.h>
#include <lib/common.h>
#include <stdint.h>

struct filesystem * fs = NULL;

int vfs_open(struct file * filp, const char * file)
{
  return fs->fops->open(filp, file);
}

int vfs_read(struct file * filp, void * buf, size_t sz)
{
  return fs->fops->read(filp, buf, sz);
}

int vfs_init(void)
{
  fs = get_filesystem("ext2");
  if(!fs)
    return -ENXIO;
  return 0;
}
