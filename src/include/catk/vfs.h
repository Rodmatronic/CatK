#ifndef __VFS_H
#define __VFS_H

#include <catk/device.h>

int vfs_mount(char path[], struct device * blkdev);
int vfs_lookup(const char * path);
int vfs_open(struct file * filp, const char * file);
int vfs_read(struct file * filp, void * buf, size_t sz);
int vfs_init(void);

#endif
