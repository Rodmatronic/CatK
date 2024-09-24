#include <catk/fs.h>
#include <catk/debug.h>
#include <catk/printk.h>
#include <catk/device.h>
#include <catk/errno.h>
#include <catk/vfs.h>
#include <lib/common.h>
#include <stdint.h>

int vfs_mount(const char * path, struct device * blkdev, struct filesystem * fs)
{
  if(MAJOR(blkdev->dev) != DISKDEV_MAJOR && MAJOR(blkdev->dev) != RAMDISK_MAJOR) {
    return -EINVAL;
  }
  if(!fs) {
    return -EINVAL;
  }
  strncpy((char *)fs->mount.mount_path, path, NAME_MAX - 1);
  if(strcmp(path, "/") != 0) {
    if(vfs_exists(path) == false) {
      debug("Oh no, you fucked up somewhere in your initrd or in the filesystem driver code!\n");
      return -ENOENT;
    }
  }
  debug("Mounting %s to \"%s\"..\n", fs->name, path);
  int rc = fs->fsops->mount(fs, blkdev);
  if(IS_ERR(rc)) {
    return rc;
  }
  return 0;
}
