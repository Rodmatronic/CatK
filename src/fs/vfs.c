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

static bool is_devfs(const char * path)
{
  size_t len = strlen(path);
  // duplicate the string
  char * str = strdup((char *)path);
  char * token;
  strncpy(str, path, len);
  if(str[0] == '/')
    str++; /* skip over '/' */
  token = strtok(str, "/");
  char * prefix = (char *)strchr(get_filesystem("devfs")->mount->mount_path, '/');
  if(!strcmp(token, prefix))
    return true;
  return false;
}

int vfs_open(struct file * filp, const char * file)
{
  debug("VFS: Opening file %s\n", file);
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

int vfs_init(void)
{
  rootfs = get_filesystem("ext2");
  if(!rootfs)
    return -ENXIO;
  return 0;
}
