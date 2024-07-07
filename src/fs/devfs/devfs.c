#include <catk/fs.h>
#include <catk/printk.h>
#include <catk/errno.h>
#include <catk/device.h>
#include <catk/ext2.h>
#include <catk/debug.h>
#include <catk/mem.h>
#include <lib/common.h>
#include <lib/ctype.h>

static struct filesystem * devfs = NULL;
static struct device * blkdev;

struct fs_operations devfs_ops;
struct file_operations devfs_file_ops;

static int devfs_mount(struct filesystem * fs, struct device * dev)
{
  debug("[devfs] mounting devfs to %s on block %d,%d\n", fs->mount->mount_path, dev->major, dev->minors);
  devfs = fs;
  return 0;
}

int devfs_init(void)
{
  return register_filesystem("devfs", &devfs_ops, &devfs_file_ops, FS_MOUNT_KERNEL);
}

struct file_operations devfs_file_ops = {
  
};

struct fs_operations devfs_ops = {
  NULL,
  NULL,
  NULL,
  devfs_mount,
};
