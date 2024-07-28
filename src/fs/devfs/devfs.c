#include <catk/fs.h>
#include <catk/printk.h>
#include <catk/errno.h>
#include <catk/device.h>
#include <catk/ext2.h>
#include <catk/debug.h>
#include <catk/mem.h>
#include <lib/common.h>
#include <lib/ctype.h>
#include <config.h>

static struct filesystem * devfs = NULL;
static struct device * blkdev;

struct fs_operations devfs_ops;
struct file_operations devfs_file_ops;

static struct inode * devfs_find_device(const char * name)
{
  return NULL;
}

static int devfs_mount(struct filesystem * fs, struct device * dev)
{
  debug("Mounting devfs to %s on block %d,%d\n", fs->mount->mount_path, dev->major, dev->minors);
  devfs = fs;
  blkdev = dev;
  return 0;
}

struct device * devfs_find_node_by_name(const char * name)
{
  return NULL; /* we'll return null for now */
}

int devfs_open(struct file * filp, const char * path)
{
  char * fn = strdup(path);
  if(fn[0] == '/')
    fn++;
  char * token = strtok(fn, "/");
  while(1)
  {
    struct device * dev;
    token = strtok(NULL, "/");
    if(!token)
      break;
    dev = devfs_find_node_by_name(token);
    if(!dev)
      return -ENOENT;  
    
  }
  free(fn);
  return 0;
}

int devfs_readdir(struct file * filp, struct dirent * dirp, size_t count)
{
  for(;;);
  return 0;
}

int devfs_init(void)
{
#if CATK_DEVFS == 1
  return register_filesystem("devfs", &devfs_ops, &devfs_file_ops, FS_MOUNT_KERNEL);
#else
  return -ENOSYS;
#endif
}

struct file_operations devfs_file_ops = {
  NULL,
  NULL,
  NULL,
  devfs_readdir,
  NULL,
  devfs_open,
  NULL
};

struct fs_operations devfs_ops = {
  NULL,
  NULL,
  NULL,
  devfs_mount,
};
