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

struct fs_operations devfs_ops;
struct file_operations devfs_file_ops;

static struct devfs_sb superblock;

static struct inode * root_inode = NULL;
static struct inode * devfs_inodes[CATK_DEVFS_INODES_MAX] = {
  NULL
};

static struct inode * inode_cache[4] = {
  NULL
};

static inline struct devfs_inode * devfs_alloc_inode(void) {
  return (struct devfs_inode *)malloc(sizeof(struct devfs_inode));
}

static inline struct file * dev2file(struct device * dev) {
  struct devfs_inode * inode = NULL;
  for(int i = 0; i < CATK_DEVFS_INODES_MAX; i++) {
    inode = ((struct devfs_inode *)devfs_inodes[i]->u.generic_ino);
    if(!memcmp(inode->dev, dev, sizeof(struct device))) {
      struct file * filp = (struct file *)malloc(sizeof(struct file));
      filp->inode = devfs_inodes[i];
      strcpy(filp->name, dev->name);
      filp->ops = dev->fops;
      return filp;
    }
  }
  return NULL;
}

static void devfs_superblock_fill(void) {
  superblock.magic = DEVFS_SUPERBLOCK_MAGIC;
  superblock.flags = 0x00;
  superblock.max_inodes = CATK_DEVFS_INODES_MAX;
  superblock.total_inodes = 0;
}

static void devfs_superblock_dump(void) {
  debug("devfs: superblock dump:\n");
  debug("\tmagic: 0x%04x\n", superblock.magic);
  debug("\tflags: 0x%02x\n", superblock.flags);
  debug("\tmax inodes: 0x%04x\n", superblock.max_inodes);
  debug("\ttotal inodes: 0x%04x\n", superblock.total_inodes);
}

static struct inode * _unused_ inode_cache_request(struct devfs_inode * inode) {
  for(int i = 0; i < 4; i++) {
    struct devfs_inode * _inode = ((struct devfs_inode *)inode_cache[i]->u.generic_ino);
    if(!memcmp(inode, _inode, sizeof(struct devfs_inode))) {
      return inode_cache[i];
    }
  }
  return NULL;
}

static void _unused_ inode_cache_flush(void) {
  struct device * cache_dev = NULL;
  struct device * dev = NULL;
  for(int i = 0; i < 4; i++) {
    cache_dev = ((struct devfs_inode *)inode_cache[i]->u.generic_ino)->dev;
    for(int j = 0; j < CATK_DEVFS_INODES_MAX; j++) {
      dev = ((struct devfs_inode *)devfs_inodes[i]->u.generic_ino)->dev;
      if(!memcmp((void *)cache_dev, (void *)dev, sizeof(struct device))) {
        devfs_inodes[j] = inode_cache[i];
        inode_cache[i] = NULL;
        break;
      }
    }
  }
}

static void inode_cache_delete(void) {
  memset((void *)inode_cache, 0, sizeof(struct inode) * 4);
}

static int devfs_get_free_inode_slot(void) {
  for(int i = 0; i < CATK_DEVFS_INODES_MAX; i++) {
    if(!devfs_inodes[i]) {
      return i;
    }
  }
  return -1;
}

static int devfs_add_inode(struct inode * ino) {
  int slot = devfs_get_free_inode_slot();
  if(slot < 0) {
    return -1;
  }
  devfs_inodes[slot] = root_inode;
  superblock.total_inodes++;
  return 0;
}

static int devfs_create_inode(struct inode * inode, mode_t mode, uid_t uid, gid_t gid) {
  inode = (struct inode *)malloc(sizeof(struct inode));
  if(!inode)
    return -ENOMEM;
  inode->mode = mode;
  inode->uid = 0;
  inode->gid = 0;
  return 0;
}

static void devfs_free_inodes(void) {
  for(int i = 0; i < CATK_DEVFS_INODES_MAX; i++) {
    if(devfs_inodes[i]) {
      free(devfs_inodes[i]);
      devfs_inodes[i] = NULL;
    }
  }
}

static int devfs_create_root_inode(void) {
  int rc;
  rc = devfs_create_inode(root_inode, S_IFDIR | S_IRWXU, 0, 0);
  if(IS_ERR(rc)) {
    return rc;
  }
  root_inode->u.generic_ino = (void *)devfs_alloc_inode();
  if(!root_inode->u.generic_ino) {
    return -ENOMEM;
  }
  ((struct devfs_inode *)root_inode->u.generic_ino)->dev = NULL;
  ((struct devfs_inode *)root_inode->u.generic_ino)->flags = 0x00;
  root_inode->inode = (uint32_t)(&root_inode->u.generic_ino);
  root_inode->flags = 0LU;
  if (devfs_add_inode(root_inode) < 0) {
    return -EAGAIN;
  }
  return 0;
}

static inline void devfs_clear_data(void) {
  devfs_free_inodes();
  inode_cache_delete();
}

static int devfs_mount(struct filesystem * fs, struct device * dev) {
  int rc;
  debug("devfs: Mounting devfs to %s on block %d,%d\n", fs->mount->mount_path, dev->major, dev->minors);
  devfs = fs;
  blkdev = dev;
  debug("devfs: Setting devfs-superblock values..\n");
  devfs_superblock_fill();
  debug("devfs: Populating %s..\n", devfs->mount->mount_path);
  rc = devfs_create_root_inode();
  if (IS_ERR(rc)) {
    return rc;
  }
  /* populate devfs with block devices and character devices */
  for(int i = 0; i < MAX_BLKDEV; i++) {
    struct device * dev = get_blkdev(i);
    if(dev) {
      struct inode * inode = NULL;
      rc = devfs_create_inode(inode, S_IFBLK, 0, 0);
      if(IS_ERR(rc)) {
        devfs_clear_data();
        return rc;
      }
      inode->u.generic_ino = (void *)devfs_alloc_inode();
      if(!inode->u.generic_ino) {
        devfs_clear_data();
        return -ENOMEM;
      }
      ((struct devfs_inode *)inode->u.generic_ino)->dev = dev;
      ((struct devfs_inode *)inode->u.generic_ino)->flags = 0x00;
      inode->inode = (uint32_t)(&inode->u.generic_ino);
      inode->flags = 0LU;
      devfs_add_inode(inode);
    }
  }
  for(int i = 0; i < MAX_CHRDEV; i++) {
    struct device * dev = get_chrdev(i);
    if(dev) {
      struct inode * inode = NULL;
      rc = devfs_create_inode(inode, S_IFCHR, 0, 0);
      if(IS_ERR(rc)) {
        devfs_clear_data();
        return rc;
      }
      inode->u.generic_ino = (void *)devfs_alloc_inode();
      if(!inode->u.generic_ino) {
        devfs_clear_data();
        return -ENOMEM;
      }
      ((struct devfs_inode *)inode->u.generic_ino)->dev = dev;
      ((struct devfs_inode *)inode->u.generic_ino)->flags = 0x00;
      inode->inode = (uint32_t)(&inode->u.generic_ino);
      inode->flags = 0LU;
      devfs_add_inode(inode);
    }
  }
  devfs_superblock_dump();
  return 0;
}

struct device * devfs_find_node_by_name(const char * name) {
  for(int i = 0; i < 32; i++) {
    struct device * dev = get_chrdev(i);
    if(!strcmp(dev->name, name)) {
      return dev;
    }
    dev = get_blkdev(i);
    if(!strcmp(dev->name, name)) {
      return dev;
    }
  }
  return NULL;
}

int devfs_open(struct file * filp, const char * path) {
  char * fn = strdup(path);
  if(fn[0] == '/') {
    fn++;
  }
  char * token = strtok(fn, "/");
  struct device * dev = NULL;
  while(1) {
    token = strtok(NULL, "/");
    if(!token) {
      break;
    }
    dev = devfs_find_node_by_name(token);
    if(!dev) {
      return -ENOENT;  
    }
  }
  filp = dev2file(dev);
  if(!filp)
    return -ENODEV;
  free(fn);
  return 0;
}

void devfs_close(struct file * filp) {
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
