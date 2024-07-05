#include <catk/fs.h>
#include <catk/printk.h>
#include <catk/errno.h>
#include <catk/device.h>
#include <catk/ext2.h>
#include <catk/debug.h>
#include <catk/mem.h>
#include <lib/common.h>
#include <lib/ctype.h>

struct fs_operations ext2_fs_ops;
struct file_operations ext2_file_ops;
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

static inline struct ext2_inode * ext2_inode_allocate(void)
{
  return (struct ext2_inode *)malloc(sb->inode_size);
}

static inline void ext2_inode_free(struct ext2_inode * inode)
{
  free(inode);
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

static void ext2_list_dir(struct ext2_directory * dir)
{
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
  debug("[ext2] Reading directory...\n");
  struct ext2_inode * _inode_buf = (struct ext2_inode *)malloc(priv_data.inode_size);
  ext2_read_inode(_inode_buf, inode);
  if ((_inode_buf->type & 0xf000) != EXT2_S_IFDIR)
  {
    debug("[ext2] inode is not a directory!\n");
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

static uint32_t ext2_parse_directory(struct ext2_directory * dir, char * path)
{
  uint32_t add = 0;
  while(dir->inode != 0 && add < priv_data.block_size)
  {
		char * name = (char *)malloc(dir->name_length + 1);
		name[dir->name_length] = 0;
		memcpy(name, &dir->type + 1, dir->name_length);
    if (strcmp(name, path) == 0)
    {
      return dir->inode;
      break;
    }
	  add += dir->size;
		dir = (struct ext2_directory *)((uint32_t)dir + dir->size);
    free(name);
	}
  return 0;
}

uint32_t ext2_find_file(char * fn, uint32_t dir_inode, struct ext2_inode * inode)
{
	if(fn[0] == '/')
  {
		fn++;
		dir_inode = 2;
	}
	uint32_t name_len = strlen(fn);
	if(name_len == 0) return dir_inode;
	uint8_t * buf = ext2_block_allocate();
	char * cfn = (char *)malloc(name_len + 1);
	while(*fn != 0)
  {
		uint32_t strindex = index_of('/', fn);
		substrr(0, strindex, fn, cfn);
		fn += strindex+(strindex == strlen(fn) ? 0 : 1);
		ext2_read_inode(inode, dir_inode);
		bool found = 0;
		for(int i = 0; i < 12 && !found; i++)
    {
			uint32_t block = inode->block_pointers[i];
			if (block == 0 || block > sb->total_blocks) break;
			ext2_read_block(block, buf);
			struct ext2_directory * dir = (struct ext2_directory *)buf;
			uint32_t add = 0;
			while(dir->inode != 0 && add < priv_data.block_size && !found)
      {
				char name[dir->name_length + 1];
        memset(name, '\0', dir->name_length + 1);
				name[dir->name_length] = '\0';
				memcpy(name, &dir->type + 1, dir->name_length);
				if(strcmp(name, cfn) == 0)
        {
					dir_inode = dir->inode;
					ext2_read_inode(inode, dir_inode);
					found = 1;
				}
        debug("name: %s\n", name);
        debug("name: %s\n", cfn);
				free(name);
				add += dir->size;
				dir = (struct ext2_directory *)((uint32_t)dir + dir->size);
			}
		}
		if(!found)
    {
			dir_inode = 0;
			fn += strlen(fn);
		}
	}
	free(cfn);
	ext2_block_release(buf);
	return dir_inode;
}
/* convert ext2 inode to vfs file */
static void ext2_inode2file(struct file * file, struct ext2_inode * inode, uint32_t inode_num)
{
  file->inode->uid                        = inode->user_id;
  file->inode->gid                        = inode->group_id;
  file->inode->flags                      = inode->flags;
  file->inode->length                     = priv_data.filesize_qword ? (inode->size_lower << 8) | (inode->size_high) : inode->size_lower;
  file->inode->inode                      = inode_num;
  file->inode->u.ext2_ino                 = inode;
  file->ops                               = &ext2_file_ops;
  file->inode->fsops                      = &ext2_fs_ops;
}

int ext2_open(struct file * filp, const char * file)
{
  struct ext2_inode * inode = ext2_inode_allocate();
  if(!inode)
    return -ENOMEM;
  uint32_t inode_num = ext2_find_file((char *)file, 2, inode);
  debug("inode number: %d\n", inode_num);
  if(!inode_num)
  {
    free(inode);
    return -ENOENT;
  }
  ext2_inode2file(filp, inode, inode_num);
  free(inode);
  return 0;
}

static void ext2_read_slink(uint32_t block, uint8_t * buf)
{
	uint8_t * bbuf = ext2_block_allocate();
	ext2_read_block(block, bbuf);
	uint32_t * blocks = (uint32_t *)bbuf;
	uint32_t numblocks = priv_data.block_size / sizeof(uint32_t);
	for(int i = 0; i < numblocks; i++)
  {
		if(!blocks[i])
      break;
		ext2_read_block(blocks[i], buf + i * priv_data.block_size);
	}
	ext2_block_release(bbuf);
}

static void ext2_read_dlink(uint32_t block, uint8_t * buf)
{
	uint8_t * bbuf = ext2_block_allocate();
	ext2_read_block(block, bbuf);
	uint32_t * blocks = (uint32_t *)bbuf;
	uint32_t numblocks = priv_data.block_size / sizeof(uint32_t);
	uint32_t singsize = numblocks * priv_data.block_size;
	for(int i = 0; i < numblocks; i++)
  {
		if(!blocks[i]) 
      break;
		ext2_read_block(blocks[i], buf + i * singsize);
	}
	ext2_block_release(bbuf);
}

static int ext2_read_file(struct file * filp, uint8_t * buf) 
{
    if (!filp || !filp->inode->inode) 
      return -EINVAL;
    
    struct ext2_inode * inode = (struct ext2_inode *)malloc(priv_data.block_size);
    ext2_read_inode(inode, filp->inode->inode);

    // Calculate the total number of blocks required to read
    uint32_t total_blocks = (inode->size_lower + priv_data.block_size - 1) / priv_data.block_size;
    uint32_t blocks_read = 0;

    for(int i = 0; i < 12 && blocks_read < total_blocks; i++)
    {
        uint32_t block = inode->block_pointers[i];
        if (block == 0 || block > sb->total_blocks) 
          break;
        
        ext2_read_block(block, buf + blocks_read * priv_data.block_size);
        blocks_read++;
    }

    if(inode->s_pointer && blocks_read < total_blocks)
    {
        debug("[ext2] reading s-link\n");
        ext2_read_slink(inode->s_pointer, buf + blocks_read * priv_data.block_size);
        blocks_read += priv_data.block_size / sizeof(uint32_t);
    }
    if(inode->d_pointer && blocks_read < total_blocks)
    {
        debug("[ext2] reading d-link\n");
        ext2_read_dlink(inode->d_pointer, buf + blocks_read * priv_data.block_size);
        blocks_read += (priv_data.block_size / sizeof(uint32_t)) * (priv_data.block_size / sizeof(uint32_t));
    }
    if(inode->t_pointer && blocks_read < total_blocks)
    {
      debug("[ext2] t-links are unsupported\n");
    }
    free(inode);
    return 0;
}

int ext2_read(struct file * filp, void * buf, size_t unused)
{
  ext2_read_file(filp, (uint8_t *)buf);
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
  ext2_read_dir(2);
  return 0;
}

int ext2_init(int fp_lba)
{
  ext2_start_lba = fp_lba;
  return register_filesystem("ext2", &ext2_fs_ops, &ext2_file_ops, FS_REQUIRES_DISK);
}

struct file_operations ext2_file_ops = {
  NULL,
  ext2_read,
  NULL,
  NULL,
  NULL,
  ext2_open,
  NULL
};

struct fs_operations ext2_fs_ops = {
  NULL,
  NULL,
  NULL,
  ext2_mount_fs
};

