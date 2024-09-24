#include <catk/core.h>
#include <catk/compiler.h>
#include <catk/types.h>
#include <catk/fs.h>
#include <catk/rand.h>
#include <catk/errno.h>
#include <catk/printk.h>
#include <catk/limits.h>
#include <lib/common.h>
#include <stdint.h>

static struct file_operations random_fops;

static int random_lseek(struct file * filp, size_t offset, int whence) {
  return 0;
}

static int random_read(struct file * filp, void * buf, size_t sz) {
  for(int i = 0; i < sz; i++) {
    ((uint8_t *)buf)[i] = gen_random() % UINT8_MAX;
  }
  return 0;
}

static int random_write(struct file * filp, void * buf, size_t sz) {
  return 0;
}

static int random_open(struct file * filp, const char * path) {
  return 0;
}

int random_init(void) {
  int rc;
  struct device * rand_dev = device_struct_alloc();
  if(!rand_dev) {
    return -ENOMEM;
  }
  strncpy((char *)rand_dev->name, "random", NAME_MAX - 1);
  rand_dev->dev = MKDEV(MEMDEV_MAJOR, MEMDEV_RANDOM);
  rand_dev->removable = false;
  rand_dev->priv_data = NULL;
  rc = chrdev_register(rand_dev, &random_fops);
  if(IS_ERR(rc)) {
    printk("Failed to register random: %d\n", rc);
    return rc;
  }
  return 0;
}

static struct file_operations random_fops = {
  NULL,
  random_lseek,
  random_read,
  random_write,
  NULL,
  NULL,
  random_open,
  NULL,
};
