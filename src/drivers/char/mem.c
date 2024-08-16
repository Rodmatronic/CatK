#include <catk/core.h>
#include <catk/compiler.h>
#include <catk/types.h>
#include <catk/fs.h>
#include <catk/rand.h>
#include <catk/errno.h>
#include <catk/printk.h>
#include <lib/common.h>
#include <stdint.h>

/*

static struct file_operations memdev_fops;

static int memdev_lseek(struct file * filp, size_t offset, int whence) {
  return 0;
}

static int memdev_read(struct file * filp, void * buf, size_t sz) {
  return 0;
}

static int memdev_write(struct file * filp, void * buf, size_t sz) {
  return 0;
}

static int memdev_open(struct file * filp, const char * path) {
  return 0;
}

int memdev_init(void) {
  int rc;
  struct device * mem_dev = device_struct_alloc();
  if(!mem_dev) {
    return -ENOMEM;
  }
  mem_dev->dev = MKDEV(MEMDEV_MAJOR, MEMDEV_MEM);
  mem_dev->removable = false;
  mem_dev->priv_data = NULL;
  rc = register_chrdev("mem", mem_dev, &memdev_fops);
  if(IS_ERR(rc)) {
    printk("Failed to register mem: %d\n", rc);
    return rc;
  }
  return 0;
}

static struct file_operations memdev_fops = {
  memdev_lseek,
  memdev_read,
  memdev_write,
  NULL,
  NULL,
  memdev_open,
  NULL,
};

*/
