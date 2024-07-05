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

int vfs_mount(char path[], struct device * blkdev)
{
  if(blkdev->major != DISKDEV_MAJOR)
    return -EINVAL;
  if(strcmp("/", path) == 0)
    return do_ext2_mount(path, blkdev);
  /* lets leave this for another time */
  return 0;
}
