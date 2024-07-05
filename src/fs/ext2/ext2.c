/* BACKUP FOR LATER USES */
/* Main problem with code: Allocated stuff accesses heap metadata structure */


#include <catk/fs.h>
#include <catk/printk.h>
#include <catk/errno.h>
#include <catk/device.h>
#include <catk/ext2.h>
#include <catk/debug.h>
#include <catk/mem.h>
#include <lib/common.h>
#include <lib/ctype.h>

struct fs_operations e2fs_ops;
static struct ext2_superblock * sb = NULL;
static struct filesystem * e2fs = NULL;
static struct ext2_priv_data priv_data;

static int ext2_start_lba = 0;

static inline uint8_t * ext2_block_allocate(void)
{
  return (uint8_t *)malloc(priv_data.block_size);
}

static inline void ext2_block_release(uint8_t * block)
{
  free((void *)block);
}

static inline uint32_t ext2_get_block_group(uint32_t inode)
{
  return (inode - 1) / sb->inodes_per_group;
}

static inline uint32_t ext2_get_inode_index(uint32_t inode)
{
  return (inode - 1) % sb->inodes_per_group;
}

static inline uint32_t ext2_get_block_inode(uint32_t inode)
{
  return (ext2_get_inode_index(inode) * priv_data.inode_size) / priv_data.block_size;
}

static inline uint32_t ext2_block_to_sector(uint32_t block)
{
  return ext2_start_lba + priv_data.sectors_per_block * block;
}

uint8_t * ext2_read_block(uint32_t block, uint8_t * buf)
{
  e2fs->mount->blkdev->fops->lseek(NULL, ext2_block_to_sector(block), SEEK_SET);
  e2fs->mount->blkdev->fops->read(0, buf, priv_data.sectors_per_block);
  return buf;
}

static int ext2_read_inode(struct ext2_inode * buf, uint32_t inode)
{
  debug("[ext2] reading inode %d\n", inode);
  uint32_t block_group = ext2_get_block_group(inode);
  debug("[ext2] block group of inode %d is %d\n", inode, block_group);
  
  uint8_t * block = ext2_block_allocate();
  if(!block)
    return -ENOMEM;
    
  struct ext2_bgdt * bgd = (struct ext2_bgdt *)ext2_read_block(priv_data.bgdt_starting_block, block);
  uint32_t inode_block = bgd[block_group].inode_table + ext2_get_block_inode(inode);
  struct ext2_inode * _inode = (struct ext2_inode *)ext2_read_block(inode_block, block);
  
  uint32_t index = ext2_get_inode_index(inode) % (priv_data.block_size / priv_data.inode_size);
  debug("[ext2] index of inode %d is %d\n", inode, index);
  /* copy data to inode */
  memcpy(buf, (void *)&_inode[index], sizeof(struct ext2_inode));
  ext2_block_release(block);
  return 0;
}

// i think its about time we test this out..

void ext2_list_dir(struct ext2_directory * dir)
{
  debug("[EXT2] Directory name length: %d\n", dir->name_length);
	uint32_t add = 0;
	while(dir->inode != 0 && add < priv_data.block_size)
  {
		char * name = (char *)malloc(dir->name_length + 1);
		name[dir->name_length] = 0;
		memcpy(name, &dir->type + 1, dir->name_length);
		if (name[0] != '\0')
    {
      printk("%s ", name);
    }
		add += dir->size;
		dir = (struct ext2_directory *)((uint32_t)dir + dir->size);
    free(name);
	}
  printk("\n");
}

int ext2_read_dir(uint32_t inode)
{
  debug("[EXT2] Reading directory...\n");
  struct ext2_inode * _inode_buf = (struct ext2_inode *)malloc(priv_data.inode_size);
  ext2_read_inode(_inode_buf, inode);
  if ((_inode_buf->type & 0xf000) != EXT2_S_IFDIR)
  {
    debug("[EXT2] Inode is not a directory!\n");
    free(_inode_buf);
    return -ENOTDIR;
  }
  uint8_t * buf = ext2_block_allocate();
  for(int i = 0; i < 12; i++)
  {
    uint32_t block = _inode_buf->block_pointers[i];
    if (block == 0 || block > sb->total_blocks)
    {
      break;
    }
    ext2_read_block(block, buf);
    ext2_list_dir((struct ext2_directory *)buf);
  }
  ext2_block_release(buf);
  free(_inode_buf);
  return 1;
}

int ext2_lookup(struct file * filp, char * file)
{
  return 0;
}

int ext2_mount_fs(struct filesystem * fs, struct device * blkdev)
{
  debug("[ext2] mounting on block %d,%d\n", blkdev->major, blkdev->minors);
  int rc;
  uint8_t sector_data[1024];
  rc = blkdev->fops->lseek(NULL, ext2_start_lba + 2, SEEK_SET);
  if(IS_ERR(rc))
    return rc;
  rc = blkdev->fops->read(NULL, (uint8_t *)&sector_data, 1);
  if(IS_ERR(rc))
    return rc;
  sb = (struct ext2_superblock *)sector_data;
  if(sb->signature != EXT2_SUPER_MAGIC)
  {
    debug("[ext2] invalid or corrupt superblock! found 0x%04x instead of 0x%04x\n", sb->signature, EXT2_SUPER_MAGIC);
    return -EINVAL;
  }
  
  fs->sb->u.ext2_sb = sb;
  printk("Detected Ext2 version %d.%d\n", sb->version_major, sb->version_minor);
  
  priv_data.block_size = 1024 << sb->block_size;
  priv_data.fragment_size = 1024 << sb->fragment_size;
  debug("[ext2] superblock block size: %d\n", priv_data.block_size);
  
  priv_data.bgdt_starting_block = (priv_data.block_size > 1024) ? 1 : 2; // Adjusting for block size
  priv_data.inode_size = sb->inode_size;
  
  if(sb->version_major < 1)
  {
    /* ext2 revisions lower than 1 use these by default */
    printk("Ext2: Setting default values...");
    priv_data.first_inode = 11;
    priv_data.inode_size = 128;
  }
  
  printk("Volume size: %d bytes\n", DIV_ROUND_UP(priv_data.block_size * sb->total_blocks, 1024 * 1024));
  printk("Last mount path: %s\n", sb->last_mount_path);
  printk("Volume name: %s\n", sb->volume_name);
  printk("Ext2 inode size: %d bytes\n", sb->inode_size);
  
  priv_data.filesize_qword = false;
  if(sb->ro_features == 0x0002)
  {
    printk("Filesystem uses 64-bit file sizes\n");
    priv_data.filesize_qword = true;
  }
  
  priv_data.sectors_per_block = (priv_data.block_size / 512);
  fs->priv_data = (void *)&priv_data;
  fs->mount->blkdev = blkdev;
  e2fs = fs;
  
  // Read root inode
  struct ext2_inode * inode = (struct ext2_inode *)malloc(priv_data.inode_size);
  rc = ext2_read_inode(inode, EXT2_ROOT_INODE);
  if(IS_ERR(rc))
    return rc;
  ext2_read_dir(2);
  return 0;
}

int ext2_init(int fp_lba)
{
  ext2_start_lba = fp_lba;
  return register_filesystem("ext2", &e2fs_ops, FS_REQUIRES_DISK);
}

struct fs_operations e2fs_ops = {
  NULL,
  NULL,
  NULL,
  NULL,
  ext2_mount_fs
};

