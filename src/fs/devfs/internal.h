#ifndef __DEVFS_INTERNAL_H
#define __DEVFS_INTERNAL_H

#include <catk/types.h>
#include <catk/limits.h>
#include <catk/vfs.h>

/* super simple devfs superblock */

struct devfs_sb {
  uint16_t magic;
#define DEVFS_SUPERBLOCK_MAGIC 0xca72
  uint8_t flags;
#define DEVFS_FLAGS_DIRTY BIT(0) // something was changed
  uint16_t total_inodes;
  uint16_t max_inodes;
};

/* super simple devfs inode */

struct devfs_inode {
  uint8_t flags;
#define DEVFS_INODE_FLAGS_DIRTY   DEVFS_FLAGS_DIRTY // something was changed
#define DEVFS_INODE_FLAGS_FREE    BIT(1)            // should be deleted
  struct device * dev;        /* the actual device it's based off of */
};

#endif