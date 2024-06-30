#ifndef __FS_H
#define __FS_H

#include <catk/limits.h>
#include <catk/dirent.h>
#include <sys/types.h>

struct file;

struct file_operations
{
	int (*read) (struct file *, void *, size_t);
	int (*write) (struct file *, void *, size_t);
	int (*readdir) (struct file *, void *, struct dirent *, int);
	int (*ioctl) (struct file *, void *, uint16_t, uint32_t);
	int (*open) (struct file *, void *);
	void (*close) (struct file *, void *);
};

struct file
{
  mode_t mode;
  off_t pos;
  uint16_t flags;
  uint16_t count;
  const char name[NAME_MAX + 1];
  const char path[NAME_MAX + 1];
  struct file_operations * ops;
};

struct filesystem
{
  /* 
  * unions make it so that only one value can be set to a item, not multiple items
  *
  * so if i were to set x in a union to 5, and y in the same union to 10, the value of x would be 10
  *
  */
  union sb
  {
    struct ext2_sb_info * ext2_sb;
    void * other_sb;
  };
};

#endif
