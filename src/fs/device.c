#include <catk/device.h>
#include <catk/errno.h>
#include <catk/printk.h>
#include <catk/debug.h>
#include <catk/mem.h>
#include <stdint.h>

static struct device chrdevs[MAX_CHRDEV];
static struct device blkdevs[MAX_BLKDEV];

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
  memcpy(&chrdevs[major], dev, sizeof(struct device));
  strncpy((char *)chrdevs[major].name, name, NAME_MAX - 1);
  chrdevs[major].major = major;
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
  memcpy(&blkdevs[major], dev, sizeof(struct device));
  strcpy((char *)blkdevs[major].name, name);
  blkdevs[major].major = major;
	blkdevs[major].fops = fops;
	return 0;
}

struct device * get_blkdev(uint8_t major)
{
	if (major >= MAX_BLKDEV)
		return NULL;
  struct device * dev = &blkdevs[major];
  if(!dev->major && !dev->minors)
    return NULL;
  return dev;
}

struct device * get_chrdev(uint8_t major)
{
  if(major >= MAX_CHRDEV)
    return NULL;
  struct device * dev = &chrdevs[major];
  if(!dev->major && !dev->minors)
    return NULL;
  return dev;
}

void device_init(void)
{
  memset((void *)chrdevs, 0, sizeof(struct device) * MAX_CHRDEV);
  memset((void *)blkdevs, 0, sizeof(struct device) * MAX_BLKDEV);
}
