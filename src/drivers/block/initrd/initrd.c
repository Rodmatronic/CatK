#include <stdint.h>
#include <catk/device.h>
#include <catk/errno.h>
#include <catk/printk.h>
#include <catk/debug.h>
#include <catk/mem.h>
#include <catk/math.h>
#include <multiboot2.h>
#include <lib/common.h>

static uintptr_t * initrd_start, * initrd_end;
struct file_operations initrd_fops;

static void initrd_read_single_sector(uint8_t * buf, int lba);

int initrd_probe(void)
{
  /* get module using multiboot2 stuff */
  struct multiboot_tag_module * mod = (struct multiboot_tag_module *)multiboot2_locate_tag(multiboot2_get_mbi(), MULTIBOOT_TAG_TYPE_MODULE);
  if(!mod) {
    printk("No initrd loaded.\n");
    return -ENODEV;
  }
  initrd_start = (void *)mod->mod_start;
  initrd_end   = (void *)mod->mod_end;
  debug("Initrd info:\n");
  debug("\tAddress\t[0x%08x - 0x%08x]\n", initrd_start, initrd_end);
  debug("\tSize:  \t%d MiB\n", DIV_ROUND_UP(((uint32_t)initrd_end - (uint32_t)initrd_start), 1048576));
  /* setup device */
  struct device * initrd = device_struct_alloc();
  if(!initrd) {
    return -ENOMEM;
  }
  snprintf((char *)initrd->name, NAME_MAX - 1, "initrd");
  initrd->dev        = MKDEV(RAMDISK_MAJOR, 0);
  initrd->removable  = true;
  initrd->priv_data  = (void *)mod;
  return blkdev_register(initrd, &initrd_fops);
}

static void initrd_read_single_sector(uint8_t * buf, int lba)
{
  uint32_t offset = lba * 512;
  memcpy(buf, (uint8_t *)(initrd_start + offset), 512);
}

static void initrd_read_sectors(uint8_t * buf, int lba, size_t sectors)
{
	for(size_t i = 0; i < sectors; i++)
	{
		initrd_read_single_sector(buf + i * 512, lba + i);
	}
}

int initrd_find_first_partition(void)
{
  return 0;
}

int initrd_dev_read(struct file * file, void * buf, size_t sz)
{
  if(file == NULL) {
    return -EFAULT;
  }
  initrd_read_sectors((uint8_t *)buf, file->fpos, sz);
  return 0;
}

int initrd_dev_write(struct file _unused_ * file, void _unused_ * buf, size_t _unused_ sz)
{
  return -EIO;
}

int initrd_dev_open(struct file _unused_ * file, const char _unused_ * unused)
{
  return 0;
}

void initrd_dev_close(struct file _unused_ * file)
{
  return;
}

int initrd_dev_lseek(struct file * file, size_t offset, int whence)
{
  if(file == NULL) {
    return -EINVAL;
  }
  switch(whence)
  {
    case SEEK_SET:
    {
      file->fpos = offset;
      break;
    }
    case SEEK_CUR:
    {
      file->fpos += offset;
      break;
    }
    case SEEK_END:
    {
      return -ESPIPE; /* not implemented yet */
      break;
    }
  }
  return 0;
}

struct file_operations initrd_fops = {
  initrd_find_first_partition,  /* firstpart */
  initrd_dev_lseek,             /* lseek */
  initrd_dev_read,              /* read */
  initrd_dev_write,             /* write */
  NULL,                         /* readdir */
  NULL,                         /* ioctl */
  initrd_dev_open,              /* open */
  initrd_dev_close,             /* close */
};
