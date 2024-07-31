#ifndef __DEVFS_INTERNAL_H
#define __DEVFS_INTERNAL_H

#include <catk/types.h>
#include <catk/vfs.h>

struct devfs_inode {
  struct device * dev;    /* the actual device it's based off of */
  struct inode vfs_inode; /* vfs inode */
};

#endif