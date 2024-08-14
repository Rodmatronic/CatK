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

int register_chrdev(const char * name, struct device * dev, struct file_operations * fops)
{
  if(MAJOR(dev->dev) != MEMDEV_MAJOR)
    debug("Registering character device \"%s%d\" (major %d)\n", name, MINOR(dev->dev), MAJOR(dev->dev));
  else
    debug("Registering character device \"%s\" (major %d)\n", name, MAJOR(dev->dev));
  if(MAJOR(dev->dev) >= MAX_CHRDEV)
    return -EINVAL;
  if(chrdevs[MAJOR(dev->dev)].fops)
    return -EBUSY;
  memcpy(&chrdevs[MAJOR(dev->dev)], dev, sizeof(struct device));
  strncpy((char *)chrdevs[MAJOR(dev->dev)].name, name, NAME_MAX - 1);
	chrdevs[MAJOR(dev->dev)].fops = fops;
  return 0;
}

int register_blkdev(const char * name, struct device * dev, struct file_operations * fops)
{
  debug("Registering block device \"%s%d\" (major %d)\n", name, MINOR(dev->dev), MAJOR(dev->dev));
	if (MAJOR(dev->dev) >= MAX_BLKDEV)
		return -EINVAL;
	if (blkdevs[MAJOR(dev->dev)].fops)
		return -EBUSY;
  memcpy(&blkdevs[MAJOR(dev->dev)], dev, sizeof(struct device));
  strcpy((char *)blkdevs[MAJOR(dev->dev)].name, name);
	blkdevs[MAJOR(dev->dev)].fops = fops;
	return 0;
}

struct device * get_blkdev(uint8_t major)
{
	if (major >= MAX_BLKDEV)
		return NULL;
  struct device * dev = &blkdevs[major];
  if(!MAJOR(dev->dev) && !MINOR(dev->dev))
    return NULL;
  return dev;
}

struct device * get_chrdev(uint8_t major)
{
  if(major >= MAX_CHRDEV)
    return NULL;
  struct device * dev = &chrdevs[major];
  if(!MAJOR(dev->dev) && !MINOR(dev->dev))
    return NULL;
  return dev;
}

void device_init(void)
{
  memset((void *)chrdevs, 0, sizeof(struct device) * MAX_CHRDEV);
  memset((void *)blkdevs, 0, sizeof(struct device) * MAX_BLKDEV);
}
