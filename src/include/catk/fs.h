#ifndef __FS_H
#define __FS_H

#include <catk/limits.h>
#include <catk/dirent.h>
#include <catk/types.h>
#include <catk/device.h>
#include <catk/fat32.h>
#include <catk/ext2.h>
#include <stdint.h>

#define NR_FILESYSTEMS 4 /* we support only 4 for now. im not sure what those 4 will be.. */

struct fs_mount
{
  char mount_path[NAME_MAX];
  struct device * blkdev;
  int flags;
};

#define FS_MOUNT_DISK    BIT(0)  /* a real filesystem that needs to be mounted on a disk */
#define FS_MOUNT_RAM     BIT(1)  /* psuedo-filesystem that's mounted by the kernel */

struct filesystem
{
  char name[NAME_MAX];
  struct fs_operations * fsops;
  struct file_operations * fops;
  struct superblock * sb;
  struct fs_mount mount;
  bool root_fs;
  void * priv_data;
};

struct file;

struct file_operations
{
  int (*firstpart)(void); // <-- disk devices should have this. non-disk devices should never have this
  int (*lseek)(struct file *, size_t, int);
	int (*read) (struct file *, void *, size_t);
	int (*write) (struct file *, void *, size_t);
	int (*readdir) (struct file *, struct dirent *, size_t);
	int (*ioctl) (struct file *, void *, uint16_t, uint32_t);
	int (*open) (struct file *, const char *);
	void (*close) (struct file *);
};

struct inode;

struct fs_operations
{
  struct inode * (*namei)(const char *);
  int (*exists)(const char *);
	int (*read_inode)(uint32_t, struct inode *);
	int (*write_inode)(struct file *);
  int (*read_block)(struct filesystem *, void *, uint32_t);
  int (*mount)(struct filesystem *, struct device *);
};

struct superblock
{
	union /* only one of these values can be set at a time */
  {
    struct ext2_superblock ext2_sb;
		void * generic_sbp;
	} u;
};


struct file
{
  char name[NAME_MAX];
  struct inode * inode;
  struct file_operations * ops;
};

struct inode
{
	mode_t mode;
	uid_t uid;
	gid_t gid;
	uint32_t flags;
	uint32_t inode;
	uint32_t length;
  union /* filesystem defined inode structure */
  {
    struct ext2_inode ext2_ino;
    void * generic_ino;
  } u;
  struct fs_operations * fsops;
};

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

int register_filesystem(const char * name, struct fs_operations * fsops, struct file_operations * fops, int flags);
struct filesystem * get_filesystem(const char * name);
int filesystems_init(int first_partition_lba);
bool is_devfs(const char * path);
void devfs_ls(void);

#endif
