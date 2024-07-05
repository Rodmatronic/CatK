#include <catk/fs.h>
#include <catk/fat32.h>
#include <catk/ext2.h>
#include <catk/debug.h>
#include <catk/printk.h>
#include <catk/errno.h>
#include <lib/common.h>
#include <stdint.h>

struct filesystem filesystems[NR_FILESYSTEMS];
int num_fs = 0;

int register_filesystem(const char * name, struct fs_operations * fsops, struct file_operations * fops, int flags)
{
  if(!fsops)
    return -EINVAL;
  if(num_fs >= NR_FILESYSTEMS)
    return -EAGAIN;
  if(flags & FS_REQUIRES_DISK && flags & FS_MOUNT_KERNEL)

  strncpy((char *)filesystems[num_fs].name, name, NAME_MAX); /* memory safety :) */
  filesystems[num_fs].fsops         = fsops;
  filesystems[num_fs].fops          = fops;
  filesystems[num_fs].mount->flags  = flags;
  /* pro programmer here B^) */
  printk("VFS: Registered filesystem \"%s\"\n", name);
  num_fs++;
  return 0;
}

struct filesystem * get_filesystem(const char * name)
{
  for(int i = 0; i < NR_FILESYSTEMS; i++)
  {
    if(!strncmp(filesystems[i].name, name, strlen(filesystems[i].name)))
      return &filesystems[i];
  }
  printk("What kind of filesystem are you looking for!?!?\n");
  debug("I'm looking for fizzbuzz.\n");
  return NULL;
}

int filesystems_init(int first_partition_lba)
{
  memset(&filesystems, 0, sizeof(filesystems));
  int rc = ext2_init(first_partition_lba);
  if(IS_ERR(rc))
    return rc;
  return 0;
}
