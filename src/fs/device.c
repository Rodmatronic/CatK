#include <catk/device.h>
#include <catk/errno.h>
#include <catk/printk.h>
#include <catk/debug.h>
#include <catk/mem.h>
#include <stdint.h>

int register_chrdev(const char * name, struct device * dev, struct file_operations * fops)
{
  return 0;
}

int register_blkdev(const char * name, struct device * dev, struct file_operations * fops)
{
	return 0;
}
