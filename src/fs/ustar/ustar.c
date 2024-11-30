#include <catk/fs.h>
#include <catk/printk.h>
#include <catk/errno.h>
#include <catk/device.h>
#include <catk/debug.h>
#include <catk/mem.h>
#include <catk/math.h>
#include <multiboot2.h>
#include <lib/common.h>
#include <lib/tar.h>

/*
* Now, I know that USTAR isn't really a filesystem
* but for initrd files, it should be treated as one.
*/

struct fs_operations ustar_fs_ops;
struct file_operations ustar_file_ops;

static struct filesystem * ustar_fs = NULL;
static struct device * blkdev = NULL;

static uint8_t * tar = NULL;

static int oct2bin(const uint8_t * str, int size) {
  int n = 0;
  for(int i = 0; size > 0; size--, i++) {
    n *= 8;
    n += str[i] - '0';
  }
  return n;
}

static inline int ustar_parse_size(const uint8_t * octal) {
  return oct2bin(octal, 11);
}

static int ustar_read_file(struct file * file, uint8_t * buffer, size_t sz) {
  if(file == NULL || buffer == NULL) {
    return -EFAULT;
  }
  uint8_t * ptr = (uint8_t *)tar;
  uint8_t * buf = NULL;
  while (!memcmp(ptr + 257, TAR_MAGIC, TAR_MAGIC_LEN)) {
    int filesize = ustar_parse_size(ptr + 0x7c);
    if(!strcmp(file->name, (const char *)ptr + 1)) {
      buf = ptr + TAR_BLOCK_SIZE;
      memcpy(buffer, buf + file->fpos, sz);
      return 0;
    }
    ptr += (((filesize + TAR_BLOCK_SIZE - 1) / TAR_BLOCK_SIZE) + 1) * TAR_BLOCK_SIZE;
  }
  return -ENOENT;
}

static struct inode * ustar_namei(const char * fn) {
  uint8_t * ptr = (uint8_t *)tar;
  int inode_num = 0;
  while (!memcmp(ptr + 257, TAR_MAGIC, TAR_MAGIC_LEN)) {
    int filesize = ustar_parse_size(ptr + 0x7c);
    if(!strcmp(fn, (const char *)ptr + 1)) {
      struct tar_inode * tar_inode = (struct tar_inode *)ptr;
      struct inode * inode = (struct inode *)malloc(sizeof(struct inode));
      assert(inode != NULL);
      /* convert USTAR inode to vfs node */
      inode->mode = (uint32_t)oct2bin((const uint8_t *)tar_inode->mode, 7);
      inode->uid = (uint32_t)oct2bin((const uint8_t *)tar_inode->uid, 7);
      inode->gid = (uint32_t)oct2bin((const uint8_t *)tar_inode->gid, 7);
      inode->inode = inode_num;
      inode->length = filesize;
      inode->u.generic_ino = (void *)ptr;
      inode->fsops  = &ustar_fs_ops;
      return inode;
    }
    ptr += (((filesize + TAR_BLOCK_SIZE - 1) / TAR_BLOCK_SIZE) + 1) * TAR_BLOCK_SIZE;
    inode_num++;
  }
  return NULL;
}

static void inode2file(struct inode * inode, struct file * file) {
  file->inode = inode;
  file->ops   = &ustar_file_ops;
  file->rdev  = 0;
  file->fpos  = 0;
}

static int ustar_exists(const char * path) {
  struct inode * inode = ustar_namei(path);
  if(!inode) {
    return false;
  }
  free(inode);
  return true;
}

static int ustar_open(struct file * file, const char * path) {
  struct inode * inode = ustar_namei(path);
  if(!inode) {
    return -ENOENT;
  }
  inode2file(inode, file);
  strncpy(file->name, path, NAME_MAX - 1);
  return 0;
}

static void ustar_close(struct file * file) {
  free(file->inode);
  free(file);
}

static int ustar_read(struct file * filp, void * buf, size_t sz) {
  return ustar_read_file(filp, (uint8_t *)buf, sz);
}

static int ustar_write(struct file _unused_ * filp, void  _unused_ * buf, size_t _unused_ sz) {
  return -EROFS; /* Read-only filesystem */
}

static int ustar_lseek(struct file * file, size_t offset, int whence)
{
  if(file == NULL) {
    return -EINVAL;
  }
  switch(whence)
  {
    case SEEK_SET:
    {
      file->fpos = offset;
      break;
    }
    case SEEK_CUR:
    {
      file->fpos += offset;
      break;
    }
    case SEEK_END:
    {
      return -ESPIPE; /* not implemented yet */
      break;
    }
  }
  return 0;
}

static int ustar_mount(struct filesystem * fs, struct device * dev) {
  int rc;
  debug("USTAR: Mounting on block %d,%d\n", MAJOR(dev->dev), MINOR(dev->dev));
  uint8_t * sb = (uint8_t *)malloc(TAR_BLOCK_SIZE);
  struct file disk_file;
  dev2file(dev, &disk_file);
  /* reset seek if not done already */
  rc = dev->fops->lseek(&disk_file, 0, SEEK_SET);
  if(IS_ERR(rc)) {
    free(sb);
    return rc;
  }
  rc = dev->fops->read(&disk_file, sb, 1);
  if(IS_ERR(rc)) {
    free(sb);
    return rc;
  }
  if(memcmp((char *)(sb + 257), TAR_MAGIC, TAR_MAGIC_LEN) != 0) {
    free(sb);
    return -EINVAL;
  }
  debug("USTAR signature is valid.\n");
  ustar_fs = fs;
  blkdev = dev;
  ustar_fs->priv_data = blkdev->priv_data;
  tar = (uint8_t *)((struct multiboot_tag_module *)ustar_fs->priv_data)->mod_start;
  free(sb);
  return 0;
}

int ustar_init(void) {
  return register_filesystem("ustar", &ustar_fs_ops, &ustar_file_ops, FS_MOUNT_DISK);
}

struct file_operations ustar_file_ops = {
  NULL,
  ustar_lseek,
  ustar_read,
  ustar_write,
  NULL,
  NULL,
  ustar_open,
  ustar_close
};

struct fs_operations ustar_fs_ops = {
  ustar_namei,
  ustar_exists,
  NULL,
  NULL,
  NULL,
  ustar_mount
};
