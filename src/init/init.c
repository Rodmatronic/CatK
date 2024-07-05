#include <catk/printk.h>
#include <catk/kernel.h>
#include <catk/debug.h>
#include <catk/vfs.h>
#include <catk/mem.h>
#include <catk/errno.h>

#define MAX

char init_path[NAME_MAX + 1]; /* either set by cmdline or set by the kernel */

int start_init(void)
{
  int rc;
  debug("[kernel] %s start. be ready for every (possible) last minute bug.\n", __FUNCTION__);
  printk("Getting ready for init process.. Everybody, put on your safety helmets.\n");
  struct file * file = (struct file *)malloc(sizeof(struct file));
  rc = vfs_open(file, "/init.sh");
  if(IS_ERR(rc))
    return rc;
  uint8_t * buf = (uint8_t *)malloc(file->inode->length);
  printk("size of file: %d bytes\n", file->inode->length);
  rc = vfs_read(file, buf, file->inode->length);
  if(IS_ERR(rc))
    return rc;
  printk("%s\n", buf);
  return 0;
}
