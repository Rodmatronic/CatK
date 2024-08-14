#include <stdint.h>
#include <catk/device.h>
#include <catk/errno.h>
#include <catk/printk.h>
#include <catk/debug.h>
#include <catk/mem.h>
#include <catk/math.h>
#include <multiboot2.h>
#include <lib/common.h>

uint32_t * ramdisk_start, * ramdisk_end;
struct file_operations ramdisk_fops;
struct device * ramdisk;

int lseek = 0;

static void ramdisk_read_single_sector(uint8_t * buf, int lba);

int ramdisk_probe(uint32_t addr)
{
  /* get module using multiboot2 stuff */
  struct multiboot_tag_module * mod = (struct multiboot_tag_module *)multiboot2_locate_tag(addr, MULTIBOOT_TAG_TYPE_MODULE);
  if(!mod)
    return -ENODEV;
  ramdisk_start = (void *)mod->mod_start;
  ramdisk_end   = (void *)mod->mod_end;
  printk("Ramdisk info:\n");
  printk("\tAddress\t[0x%08x - 0x%08x]\n", ramdisk_start, ramdisk_end);
  printk("\tSize:  \t%d MiB\n", DIV_ROUND_UP(((uint32_t)ramdisk_end - (uint32_t)ramdisk_start), 1048576));
  ramdisk->dev        = MKDEV(RAMDISK_MAJOR, 0);
  ramdisk->removable  = false;
  ramdisk->priv_data  = (void *)mod;
  return register_blkdev("ramdisk", ramdisk, &ramdisk_fops);
}

static void ramdisk_write_single_sector(uint8_t * buf, int lba)
{
  uint32_t offset = lba * 512;
  uint8_t * mem = (uint8_t *)(ramdisk_start + offset);
  memcpy(mem, buf, 512);
}

static void ramdisk_read_single_sector(uint8_t * buf, int lba)
{
  uint32_t offset = lba * 512;
  uint8_t * mem = (uint8_t *)(ramdisk_start + offset);
  memcpy(buf, mem, 512);
}

static void ramdisk_read_sectors(uint8_t * buf, int lba, size_t sectors)
{
	for(int i = 0; i < sectors; i++)
	{
		ramdisk_read_single_sector(buf + i * 512, lba + i);
	}
}

static void ramdisk_write_sectors(uint8_t * buf, int lba, size_t sectors)
{
	for(int i = 0; i < sectors; i++)
	{
		ramdisk_write_single_sector(buf + i * 512, lba + i);
	}
}

int ramdisk_find_first_partition(void)
{
  return 0;
}

int ramdisk_dev_read(struct file * file, void * buf, size_t sz)
{
  ramdisk_read_sectors((uint8_t *)buf, lseek, sz);
  return 0;
}

int ramdisk_dev_write(struct file * file, void * buf, size_t sz)
{
  ramdisk_write_sectors((uint8_t *)buf, lseek, sz);
  return 0;
}

int ramdisk_dev_open(struct file * file, const char * unused)
{
  return 0;
}

void ramdisk_dev_close(struct file * file)
{
  return;
}

int ramdisk_dev_lseek(struct file * file, size_t offset, int whence)
{
  switch(whence)
  {
    case SEEK_SET:
    {
      lseek = offset;
      break;
    }
    case SEEK_CUR:
    {
      lseek += offset;
      break;
    }
    case SEEK_END:
    {
      return -ENOSYS; /* not implemented yet */
      break;
    }
  }
  return 0;
}

struct file_operations ramdisk_fops = {
  ramdisk_dev_lseek,        /* lseek */
  ramdisk_dev_read,         /* read */
  ramdisk_dev_write,        /* write */
  NULL,                     /* readdir */
  NULL,                     /* ioctl */
  ramdisk_dev_open,         /* open */
  ramdisk_dev_close,        /* close */
};
