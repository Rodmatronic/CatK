#include <catk/fs.h>
#include <catk/debug.h>
#include <catk/printk.h>
#include <catk/device.h>
#include <catk/errno.h>
#include <lib/common.h>
#include <stdint.h>

struct filesystem * rootfs = NULL;

static int do_ext2_mount(char * path, struct device * blkdev)
{
  int rc;
  rootfs = get_filesystem("ext2");
  if(!rootfs)
    return -EAGAIN;
  strncpy(rootfs->mount->mount_path, path, DEVNAME_MAX);
  rc = rootfs->fsops->mount(rootfs, blkdev);
  if(IS_ERR(rc))
    return rc;
  return 0;
}

static int do_devfs_mount(char * path, struct device * blkdev)
{
  int rc;
  struct filesystem * devfs = NULL;
  devfs = get_filesystem("devfs");
  if(!devfs)
    return -EAGAIN;
  strncpy(devfs->mount->mount_path, path, DEVNAME_MAX);
  rc = devfs->fsops->mount(devfs, blkdev);
  if(IS_ERR(rc))
    return rc;
  return 0;
}

int vfs_mount(char * path, struct device * blkdev)
{
  if(blkdev->major != DISKDEV_MAJOR)
  {
    if(blkdev->major != RAMDISK_MAJOR)
      return -EINVAL;
  }
  if(strncmp("/", path, strlen(path)) == 0)
    return do_ext2_mount(path, blkdev);
  else if(strncmp("/dev", path, strlen(path)) == 0)
    return do_devfs_mount(path, blkdev);
  return 0;
}
