#ifndef __DEVICE_H
#define __DEVICE_H

#include <catk/bitops.h>
#include <catk/tty.h>
#include <catk/types.h>
#include <catk/fs.h>
#include <lib/common.h>

#define MAX_BLKDEV 32
#define MAX_CHRDEV 32

/* supported majors */
/* more info listed here: https://www.kernel.org/doc/Documentation/admin-guide/devices.txt */

#define NULL_MAJOR        0
#define NULL_MINOR        0

#define MEMDEV_MAJOR      1     // char 
#define DISKDEV_MAJOR     3     // block
#define TTYDEV_MAJOR      4     // char
#define FBDEV_MAJOR       29    // char
/* memdev minors */
#define MEMDEV_MEM        1
#define MEMDEV_NULL       3
#define MEMDEV_PORT       4
#define MEMDEV_ZERO       5
#define MEMDEV_FULL       7
#define MEMDEV_RANDOM     8
#define MEMDEV_URANDOM    9
#define MEMDEV_KMSG       11
/* most harddisk minors are just the amount of partitions */
#define DISKDEV_MASTER    0
#define DISKDEV_SLAVE     64
/* tty minors */
#define TTYDEV_VC         0     // this means its a console device
#define TTYDEV_SERIAL     64
/* framebuffer minors are just the amount of framebuffers */

#define SET_MINOR(minors, bit)   ((minors[(bit) / 32]) |= (1 << ((bit) % 32)))
#define CLEAR_MINOR(minors, bit) ((minors[(bit) / 32]) &= ~(1 << ((bit) % 32)))
#define TEST_MINOR(minors, bit)	 ((minors[(bit) / 32]) & (1 << ((bit) % 32)))

struct tty_struct;

/* basic device structure */
struct device
{
  const char * name;                    /* initial name of device */
  uint8_t major;                        /* acts as a type */
  uint8_t minors;                       /* acts as a class / classes */
  bool removable;                       /* can it be removed? */
  struct device * parent;               /* parent of the device (if it has one) */
  struct file_operations * fops;
  void * priv_data;                     /* device's private data */
};

int register_chrdev(uint8_t major, const char * name, struct device * dev, struct file_operations * fops);
int register_blkdev(uint8_t major, const char * name, struct device * dev, struct file_operations * fops);
struct device * get_blkdev(uint8_t major);
void device_init(void);

#endif
