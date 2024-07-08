#include <catk/device.h>
#include <catk/errno.h>
#include <catk/printk.h>
#include <catk/debug.h>
#include <stdint.h>

static struct device chrdevs[MAX_CHRDEV] = {
  NULL
};

static struct device blkdevs[MAX_BLKDEV] = {
  NULL
};

int register_chrdev(uint8_t major, const char * name, struct device * dev, struct file_operations * fops)
{
  printk("Registering character device \"%s%d\" (major %d)\n", name, dev->minors, major);
  if(major >= MAX_CHRDEV)
    return -EINVAL;
  if(chrdevs[major].fops)
    return -EBUSY;
  chrdevs[major] = *dev;
  chrdevs[major].name = name;
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
  memcpy((void *)&blkdevs[major], dev, sizeof(struct device));
  strcpy((char *)blkdevs[major].name, name);
	blkdevs[major].fops = fops;
	return 0;
}

/* this stupid function doesn't even get the correct device */
struct device * get_blkdev(uint8_t major)
{
	if (major >= MAX_BLKDEV)
		return NULL;
  return &blkdevs[major];
}

struct device * get_chrdev(uint8_t major)
{
  if(major >= MAX_CHRDEV)
    return NULL;
  return &chrdevs[major];
}

void device_init(void)
{
  memset(&chrdevs, 0, sizeof(chrdevs));
  memset(&blkdevs, 0, sizeof(blkdevs));
  debug("[dev] devices can now be registered.\n");
}
