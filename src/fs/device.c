#include <catk/device.h>
#include <catk/errno.h>
#include <catk/printk.h>
#include <catk/debug.h>
#include <catk/mem.h>
#include <stdint.h>

static struct device * chrdevs[MAX_CHRDEV];
static struct device * blkdevs[MAX_BLKDEV];

struct device * device_struct_alloc(void) {
  return (struct device *)malloc(sizeof(struct device));
}

int chrdev_register(struct device * dev, struct file_operations * fops) {
  debug("Registering character device \"%s\" (%d, %d)\n", dev->name, MAJOR(dev->dev), MINOR(dev->dev));
  if(MAJOR(dev->dev) >= MAX_CHRDEV)
    return -EINVAL;
  if(chrdevs[MAJOR(dev->dev)])
    return -EBUSY;
  chrdevs[MAJOR(dev->dev)] = dev;
	chrdevs[MAJOR(dev->dev)]->fops = fops;
  return 0;
}

int blkdev_register(struct device * dev, struct file_operations * fops) {
  debug("Registering block device \"%s\" (%d, %d)\n", dev->name, MAJOR(dev->dev), MINOR(dev->dev));
	if (MAJOR(dev->dev) >= MAX_BLKDEV)
		return -EINVAL;
	if (blkdevs[MAJOR(dev->dev)])
		return -EBUSY;
  blkdevs[MAJOR(dev->dev)] = dev;
	blkdevs[MAJOR(dev->dev)]->fops = fops;
	return 0;
}

struct device * blkdev_get(uint8_t major) {
	if (major >= MAX_BLKDEV)
		return NULL;
  struct device * dev = blkdevs[major];
  if(dev == NULL) {
    return NULL;
  }
  if(!MAJOR(dev->dev) && !MINOR(dev->dev))
    return NULL;
  return dev;
}

struct device * blkdev_get_first(void) {
  for(int i = 0; i < MAX_BLKDEV; i++) {
    if(blkdevs[i] != NULL) {
      return blkdevs[i];
    }
  }
  return NULL;
}

struct device * chrdev_get(uint8_t major) {
  if(major >= MAX_CHRDEV)
    return NULL;
  struct device * dev = chrdevs[major];
  if(dev == NULL) {
    return NULL;
  }
  if(!MAJOR(dev->dev) && !MINOR(dev->dev))
    return NULL;
  return dev;
}

void dev2file(struct device * dev, struct file * file) {
  if(file == NULL) {
    /* dont mess with null pointers */
    return;
  }
  file->inode = NULL;
  strcpy(file->name, dev->name);
  file->ops   = dev->fops;
  file->rdev  = dev->dev;
  file->fpos  = 0;
}
