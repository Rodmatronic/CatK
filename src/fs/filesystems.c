#include <catk/fs.h>
#include <catk/fat32.h>
#include <catk/ext2.h>
#include <catk/debug.h>
#include <catk/printk.h>
#include <catk/errno.h>
#include <catk/ustar.h>
#include <lib/common.h>
#include <config.h>
#include <stdint.h>

struct filesystem filesystems[NR_FILESYSTEMS];
int num_fs = 0;

int register_filesystem(const char * name, struct fs_operations * fsops, struct file_operations * fops, int flags)
{
  if(!fsops)
    return -EINVAL;
  if(num_fs >= NR_FILESYSTEMS)
    return -EAGAIN;
  if(flags & FS_MOUNT_DISK && flags & FS_MOUNT_RAM) /* two of these flags cannot be set at the same time */
    return -EINVAL;
  strncpy((char *)filesystems[num_fs].name, name, NAME_MAX - 1); /* memory safety :) */
  filesystems[num_fs].fsops         = fsops;
  filesystems[num_fs].fops          = fops;
  filesystems[num_fs].mount.flags  = flags;
  /* pro programmer here B^) */
  printk("VFS: Registered filesystem \"%s\"\n", name);
  debug("Filesystem info:\n");
  debug("\tName: %s\n", filesystems[num_fs].name);
  debug("\tFlags: 0x%02x\n", flags);
  num_fs++;
  return 0;
}

struct filesystem * get_filesystem(const char * name)
{
  for(int i = 0; i < NR_FILESYSTEMS; i++)
  {
    if(!strncmp(filesystems[i].name, name, NAME_MAX - 1))
      return &filesystems[i];
  }
  debug("What kind of filesystem are you looking for!?!?\n");
  return NULL;
}

int filesystems_init(int first_partition_lba)
{
  memset((void *)filesystems, 0, sizeof(struct filesystem) * NR_FILESYSTEMS);

  /* first up are the real filesystems */
  int rc = ext2_init(first_partition_lba);
  if(IS_ERR(rc)) {
    printk("VFS: Warning: Failed to initialize Ext2: %d\n", rc);
  }

  rc = ustar_init();
  if(IS_ERR(rc)) {
    printk("VFS: Warning: Failed to initialize USTAR: %d\n", rc);
  }
  /* now for the psuedo-filesystems :) */
  rc = devfs_init();
  if(IS_ERR(rc)) {
    printk("VFS: Warning: Failed to initialize devfs: %d\n", rc);
  }

  return 0;
}
