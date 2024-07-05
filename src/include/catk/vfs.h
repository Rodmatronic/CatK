#ifndef __VFS_H
#define __VFS_H

#include <catk/device.h>

int vfs_mount(char path[], struct device * blkdev);
int vfs_open(struct file * file, char * path);

#endif
