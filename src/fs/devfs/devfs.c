#include <catk/fs.h>
#include <catk/printk.h>
#include <catk/errno.h>
#include <catk/device.h>
#include <catk/ext2.h>
#include <catk/debug.h>
#include <catk/mem.h>
#include <catk/errno.h>
#include <catk/stat.h>
#include <catk/compiler.h>
#include <lib/common.h>
#include <lib/ctype.h>
#include <config.h>

#include "internal.h"

#if CATK_DEVFS_INODES_MAX < 128
#error "Max devfs inodes is too small!"
#endif

static struct filesystem * devfs = NULL;
static struct device * blkdev = NULL;

static struct inode * inodes[CATK_DEVFS_INODES_MAX];

struct fs_operations devfs_ops;
struct file_operations devfs_file_ops;

static struct devfs_sb superblock;

static inline struct inode * devfs_inode_alloc(void) {
  return (struct inode *)malloc(sizeof(struct inode));
}

static struct inode * devfs_new_inode(mode_t mode, uid_t uid, gid_t gid) {
  struct inode * inode = devfs_inode_alloc();
  if(!inode) {
    return NULL;
  }
  inode->flags = 0x0;
  inode->mode = mode;
  inode->gid = gid;
  inode->uid = uid;
  return inode;
}

static int find_free_slot(void) {
  for(int i = 0; i < CATK_DEVFS_INODES_MAX; i++) {
    if(!inodes[i]) {
      return i;
    }
  }
  return -1;
}

static void devfs_add_inode(struct inode * inode) {
  int slot = find_free_slot();
  if(slot < 0) {
    return;
  }
  inodes[slot] = inode;
  superblock.total_inodes++;
}

static int devfs_mount(struct filesystem * fs, struct device * dev) {
  debug("devfs: Mounting devfs to %s on block %d,%d\n", fs->mount->mount_path, dev->major, dev->minors);
  devfs = fs;
  blkdev = dev;
  fs->sb->u.generic_sbp = ((void *)&superblock);
  /* create root inode */
  struct inode * root_inode = devfs_new_inode(S_IFDIR, 0, 0);
  devfs_add_inode(root_inode);
  for(int i = 0; i < MAX_BLKDEV; i++) {
    struct device * dev = get_blkdev(i);
    if(!dev) {
      continue;
    }
    struct inode * inode = devfs_new_inode(S_IFBLK, 0, 0);
    inode->u.generic_ino = malloc(sizeof(struct devfs_inode));
    ((struct devfs_inode *)inode->u.generic_ino)->dev = dev;
    ((struct devfs_inode *)inode->u.generic_ino)->flags = 0;
    devfs_add_inode(inode);
  }
  for(int i = 0; i < MAX_CHRDEV; i++) {
    struct device * dev = get_chrdev(i);
    if(!dev) {
      continue;
    }
    struct inode * inode = devfs_new_inode(S_IFCHR, 0, 0);
    inode->u.generic_ino = malloc(sizeof(struct devfs_inode));
    ((struct devfs_inode *)inode->u.generic_ino)->dev = dev;
    ((struct devfs_inode *)inode->u.generic_ino)->flags = 0;
    devfs_add_inode(inode);
  }
  return 0;
}

static struct device * devfs_find_node_by_name(const char * name) {
  for(int i = 0; i < CATK_DEVFS_INODES_MAX; i++) {
    struct devfs_inode * dnode = ((struct devfs_inode *)inodes[i]->u.generic_ino);
    if(!strncmp(dnode->dev->name, name, strlen(name))) {
      return dnode->dev;
    }
  }
  return NULL;
}

static void dev2file(struct device * dev, struct file * file) {
  file->inode = NULL;
  strcpy(file->name, dev->name);
  file->ops = dev->fops;
}

static int devfs_open(struct file * filp, const char * path) {
  char * fn = strdup(path);
  struct device * dev = NULL;
  if(fn[0] == '/') {
    fn++;
  }
  fn = strchr(fn, '/');
  while(fn) {
    if(fn[0] == '/') {
      fn++;
    }
    dev = devfs_find_node_by_name(fn);
    if(dev) {
      break;
    } else {
      free(fn);
      return -ENOENT;
    }
    fn = strchr(fn + 1, '/');
  }
  dev2file(dev, filp);
  free(fn);
  return 0;
}

static void _unused_ devfs_close(struct file * filp) {
  free(filp->inode);
}

int devfs_read(struct file * filp, void * buf, size_t sz)
{
  return filp->ops->read(filp, buf, sz);
}

int devfs_write(struct file * filp, void * buf, size_t sz)
{
  return filp->ops->write(filp, buf, sz);
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
  devfs_read,
  devfs_write,
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
