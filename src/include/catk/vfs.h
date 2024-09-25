#ifndef __VFS_H
#define __VFS_H

#include <catk/device.h>
#include <catk/fs.h>


/* file descriptor operations */
int file_desc_alloc(void);
void file_desc_free(int fd);
struct file * fd_dup(struct file * file);
/* basic vfs operations */
struct inode * namei(const char * pathname);
int vfs_mount(const char * path, struct device * blkdev, struct filesystem * fs);
int vfs_exists(const char * path);
int vfs_lookup(const char * path);
int vfs_open(struct file * filp, const char * file);
void vfs_close(struct file * filp);
int vfs_read(struct file * filp, void * buf, size_t sz);
int vfs_write(struct file * filp, void * buf, size_t sz);
void vfs_set_rootfs(struct filesystem * fs);

#endif
