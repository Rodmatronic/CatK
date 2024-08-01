#include <catk/device.h>
#include <catk/errno.h>
#include <catk/printk.h>
#include <catk/debug.h>
#include <catk/mem.h>
#include <stdint.h>

static struct device chrdevs[MAX_CHRDEV] = {
  {NULL}
};

static struct device blkdevs[MAX_BLKDEV] = {
  {NULL}
};

struct device * device_struct_alloc(void)
{
  return (struct device *)malloc(sizeof(struct device));
}

int register_chrdev(uint8_t major, const char * name, struct device * dev, struct file_operations * fops)
{
  if(major != MEMDEV_MAJOR)
    printk("Registering character device \"%s%d\" (major %d)\n", name, dev->minors, major);
  else
    printk("Registering character device \"%s\" (major %d)\n", name, dev->minors, major);
  if(major >= MAX_CHRDEV)
    return -EINVAL;
  if(chrdevs[major].fops)
    return -EBUSY;
  chrdevs[major] = *dev;
  strcpy((char *)chrdevs[major].name, name);
	chrdevs[major].fops = fops;
  return 0;
}

int register_blkdev(uint8_t major, const char * name, struct device * dev, struct file_operations * fops)
{
  printk("Registering block device \"%s%d\" (major %d)\n", name, dev->minors, major);
	if (major >= MAX_BLKDEV)
		return -EINVAL;
	if (blkdevs[major].fops)
		return -EBUSY;
  blkdevs[major] = *dev;
  strcpy((char *)blkdevs[major].name, name);
	blkdevs[major].fops = fops;
	return 0;
}

struct device * get_blkdev(uint8_t major)
{
	if (major >= MAX_BLKDEV)
		return NULL;
  struct device * dev = &blkdevs[major];
  if(!dev)
    return NULL;
  return dev;
}

struct device * get_chrdev(uint8_t major)
{
  if(major >= MAX_CHRDEV)
    return NULL;
  struct device * dev = &chrdevs[major];
  if(!dev)
    return NULL;
  return dev;
}

void device_dump(void) {
  for(int i = 0; i < MAX_CHRDEV; i++) {
    printk("%s\n", chrdevs[i].name);
  }
}

void device_init(void)
{
  memset((void *)chrdevs, 0, sizeof(struct device) * MAX_CHRDEV);
  memset((void *)blkdevs, 0, sizeof(struct device) * MAX_BLKDEV);
}
