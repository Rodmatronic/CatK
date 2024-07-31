#include <catk/core.h>
#include <catk/compiler.h>
#include <catk/types.h>
#include <catk/fs.h>
#include <stdint.h>

static struct file_operations random_fops;

static int random_lseek(struct file * filp, size_t offset, int whence) {
  return 0;
}

static int random_read(struct file * filp, void * buf, size_t sz) {
  for(int i = 0; i < sz; i++) {
    ((uint8_t *)buf)[i] = entropy() % UINT8_MAX;
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
  struct device * rand_dev = device_struct_alloc();
  rand_dev->major = MEMDEV_MAJOR;
  rand_dev->minors = MEMDEV_RANDOM;
  rand_dev->removable = false;
  rand_dev->priv_data = NULL;
  return register_chrdev(MEMDEV_MAJOR, "random", rand_dev, &random_fops);
}

static struct file_operations random_fops = {
  random_lseek,
  random_read,
  random_write,
  NULL,
  NULL,
  random_open,
  NULL,
};