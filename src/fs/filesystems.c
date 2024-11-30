#include <catk/fs.h>
#include <catk/fat32.h>
#include <catk/ext2.h>
#include <catk/debug.h>
#include <catk/printk.h>
#include <catk/errno.h>
#include <catk/ustar.h>
#include <catk/trace.h>
#include <catk/compiler.h>
#include <lib/common.h>
#include <config.h>
#include <stdint.h>

struct fs_init_entries {
  const char * name;
  void * init;
};

static struct fs_init_entries filesystem_init_funcs[NR_FILESYSTEMS] = {

#ifdef CATK_EXT2
  {"EXT2", ext2_init},
#endif

#ifdef CATK_FAT32
  {"FAT32", fat32_init},
#endif

#ifdef CATK_DEVFS
  {"devfs", devfs_init},
#endif
  
  {"ustar", ustar_init},

  {NULL, NULL}

};

static struct filesystem filesystems[NR_FILESYSTEMS];
static int num_fs = 0;

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
  debug("VFS: Registered filesystem \"%s\"\n", name);
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
  return NULL;
}

int filesystems_init(void)
{
  memset(&filesystems, 0, sizeof(struct filesystem) * NR_FILESYSTEMS);
  struct fs_init_entries * fs = &filesystem_init_funcs[0];
  for(int i = 0; filesystem_init_funcs[i].init != NULL; i++, fs = &filesystem_init_funcs[i]) {
    printk("VFS: Initializing filesystem %s...\n", fs->name);
    int (*init_func)(void) = fs->init;
    int rc = init_func();
    if(IS_ERR(rc)) {
      printk("Failed to initialize %s: %d\n", fs->name, rc);
    }
  }
  return 0;
}
