#ifndef __EXT2_H
#define __EXT2_H

#include <stdint.h>
#include <catk/compiler.h>
#include <catk/limits.h>
#include <catk/types.h>
#include <catk/fs.h>
#include <lib/common.h>

#define EXT2_SUPER_MAGIC	0xef53
#define EXT2_ROOT_INODE   2

struct ext2_superblock
{
	uint32_t total_inodes;
	uint32_t total_blocks;
	uint32_t superuser_blocks;
	uint32_t unallocated_blocks;
	uint32_t unallocated_inodes;
	uint32_t superblock_block;
	uint32_t block_size; //do 1024 << block_size to get the block size
	uint32_t fragment_size; //Do 1024 << fragment_size to get the fragment size
	uint32_t blocks_per_group;
	uint32_t fragments_per_group;
	uint32_t inodes_per_group;
	uint32_t last_mount;
	uint32_t last_write;
	uint16_t times_mounted; //Since last fsck
	uint16_t mounts_allowed; //fsck must be done after this amount surpassed
	uint16_t signature;
	uint16_t state;
	uint16_t error_action;
	uint16_t version_minor;
	uint32_t last_check; //POSIX time of last fsck
	uint32_t check_interval; //POSIX time between fscks
	uint32_t os_id;
	uint32_t version_major;
	uint16_t reserved_user;
	uint16_t reserved_group;
	//Start extended fields
	uint32_t first_inode;
	uint16_t inode_size;
	uint16_t superblock_group;
	uint32_t optional_features;
	uint32_t required_features;
	uint32_t ro_features;
	uint8_t filesystem_id[16];
	uint8_t volume_name[16];
	uint8_t last_mount_path[64];
	uint32_t compression;
	uint8_t file_prealloc_blocks;
	uint8_t directory_prealloc_blocks;
	uint16_t unused;
	uint8_t journal_id[16];
	uint32_t journal_inode;
	uint32_t journal_device;
	uint32_t orphan_inode_head;
	uint8_t extra[276];
}_packed_;

struct ext2_bgdt
{
  uint32_t block_bitmap;
  uint32_t inode_bitmap;
  uint32_t inode_table;
  uint16_t free_blocks;
  uint16_t used_blocks;
  uint16_t num_dirs;
  uint8_t unused[14];
}_packed_;

struct ext2_inode
{
	uint16_t type;
	uint16_t user_id;
	uint32_t size_lower;
	uint32_t last_access_time;
	uint32_t creation_time;
	uint32_t last_modification_time;
	uint32_t deletion_time;
	uint16_t group_id;
	uint16_t links_count;
	uint32_t blocks;
	uint32_t flags;
	uint32_t os_specific_1;
	uint32_t block_pointers[12];
	uint32_t s_pointer;
	uint32_t d_pointer;
	uint32_t t_pointer;
	uint32_t generation_number;
	uint32_t file_acl;
  uint32_t size_high;
	uint32_t block_address;
	uint32_t os_specific_2[3];
}_packed_;

struct ext2_directory
{
	uint32_t inode;
	uint16_t size;
	uint8_t name_length;
	uint8_t type;
}_packed_;

struct ext2_priv_data
{
  uint32_t block_size;
  uint32_t fragment_size;
  uint32_t first_inode;
  uint16_t inode_size;
  uint32_t sectors_per_block;
  uint32_t bgdt_starting_block;
  bool filesize_qword;
};

#define EXT2_S_IFSOCK 0xC000
#define EXT2_S_IFLNK 0xA000
#define EXT2_S_IFREG 0x8000
#define EXT2_S_IFBLK 0x6000
#define EXT2_S_IFDIR 0x4000
#define EXT2_S_IFCHR 0x2000
#define EXT2_S_IFIFO 0x1000

#define EXT2_S_ISUID 0x0800
#define EXT2_S_ISGID 0x0400
#define EXT2_S_ISVTX 0x0200

#define EXT2_S_IRUSR 0x0100
#define EXT2_S_IWUSR 0x0080
#define EXT2_S_IXUSR 0x0040
#define EXT2_S_IRGRP 0x0020
#define EXT2_S_IWGRP 0x0010
#define EXT2_S_IXGRP 0x0008
#define EXT2_S_IROTH 0x0004
#define EXT2_S_IWOTH 0x0002
#define EXT2_S_IXOTH 0x0001

struct filesystem;
struct file;
struct inode;

int ext2_init(int fp_lba);

#endif
