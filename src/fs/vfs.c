#include <catk/fs.h>
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
  int tokens = 0;
  // duplicate the string
  char * str = strdup((char *)path);
  char * token;
  strncpy(str, path, len);
  if(str[0] == '/')
    str++; /* skip over '/' */
  token = strtok(str, "/");
  if(!strcmp(token, "dev"))
    return true;
  return false;
}

int vfs_open(struct file * filp, const char * file)
{
  debug("[vfs] opening file %s\n", file);
  int rc;
  struct task * p = get_current_task();
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
  debug("[vfs] reading file %s\n", filp->name);
  return rootfs->fops->read(filp, buf, sz);
}

int vfs_init(void)
{
  rootfs = get_filesystem("ext2");
  if(!rootfs)
    return -ENXIO;
  return 0;
}
