#include <catk/core.h>
#include <catk/compiler.h>
#include <catk/types.h>
#include <catk/fs.h>
#include <catk/rand.h>
#include <stdint.h>

static struct file_operations urandom_fops;

static int urandom_lseek(struct file * filp, size_t offset, int whence) {
  return 0;
}

static int urandom_read(struct file * filp, void * buf, size_t sz) {
  for(int i = 0; i < sz; i++) {
    ((uint8_t *)buf)[i] = gen_random() % UINT8_MAX;
  }
  return 0;
}

static int urandom_write(struct file * filp, void * buf, size_t sz) {
  return 0;
}

static int urandom_open(struct file * filp, const char * path) {
  return 0;
}

int urandom_init(void) {
  struct device * urand_dev = device_struct_alloc();
  urand_dev->major = MEMDEV_MAJOR;
  urand_dev->minors = MEMDEV_URANDOM;
  urand_dev->removable = false;
  urand_dev->priv_data = NULL;
  return register_chrdev(MEMDEV_MAJOR, "urandom", urand_dev, &urandom_fops);
}

static struct file_operations urandom_fops = {
  urandom_lseek,
  urandom_read,
  urandom_write,
  NULL,
  NULL,
  urandom_open,
  NULL,
};