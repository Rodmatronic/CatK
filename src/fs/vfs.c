#include <catk/fs.h>
#include <catk/vfs.h>
#include <catk/debug.h>
#include <catk/printk.h>
#include <catk/device.h>
#include <catk/errno.h>
#include <catk/mem.h>
#include <catk/task.h>
#include <lib/common.h>
#include <stdint.h>

static struct filesystem * rootfs = NULL;

bool is_devfs(const char * path)
{
  if(path[0] == '/')
    path++;
  return (strncmp(path, "dev/", 3) == 0);
}

int vfs_exists(const char * path) {
  return rootfs->fsops->exists(path);
}

int vfs_open(struct file * filp, const char * file)
{
  debug("Opening file %s\n", file);
  int rc;
  if(is_devfs(file))
  {
    rc = get_filesystem("devfs")->fops->open(filp, file);
  }
  else
  {
    rc = rootfs->fops->open(filp, file);
  }
  if(IS_ERR(rc))
    return rc;
  return 0;
}

void vfs_close(struct file * filp) {
  if(is_devfs(filp->name)) {
    get_filesystem("devfs")->fops->close(filp);
  } else {
    rootfs->fops->close(filp);
  }
}

int vfs_read(struct file * filp, void * buf, size_t sz)
{
  if(!filp->ops->read)
    return -ENXIO;
  return filp->ops->read(filp, buf, sz);
}

int vfs_write(struct file * filp, void * buf, size_t sz)
{
  if(!filp->ops->write)
    return -ENXIO;
  return filp->ops->write(filp, buf, sz);
}

void vfs_set_rootfs(struct filesystem * fs)
{
  rootfs = fs;
}
