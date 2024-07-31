#include <stdint.h>
#include <catk/core.h>
#include <catk/device.h>
#include <catk/errno.h>
#include <catk/printk.h>
#include <catk/debug.h>
#include <catk/mem.h>
#include <catk/io.h>
#include <lib/common.h>

#include "ide.h"

#define ATA_PRIMARY     0x00
#define ATA_SECONDARY   ATA_PRIMARY + 1

static int num_ata = 0;
static struct device * ata_devices[4];
static struct ide_channel ata_channels[2];

struct file_operations ata_fops;

static int lseek = 0;

static void ata_primary_irq(struct intr_stack_frame * frame)
{
  printk("ATA PRIMARY INTERRUPT!\n");
}

static void ata_secondary_irq(struct intr_stack_frame * frame)
{
  printk("ATA SECONDARY INTERRUPT!\n");
}

int ata_finalize_init(struct ide_drive * drv, const uint32_t bar0, const uint32_t bar1, const uint32_t bar2, const uint32_t bar3, const uint32_t bar4)
{
  int rc;
  /* ata channel stuff */
  ata_channels[ATA_PRIMARY].base    = bar0;
  ata_channels[ATA_PRIMARY].ctrl    = bar1;
  ata_channels[ATA_SECONDARY].base  = bar2;
  ata_channels[ATA_SECONDARY].ctrl  = bar3;
  ata_channels[ATA_PRIMARY].bmide   = bar4;
  ata_channels[ATA_SECONDARY].bmide = bar4 + 8;
  debug("ATA: Primary channel base: 0x%08x\n", ata_channels[ATA_PRIMARY].base);
  debug("ATA: Secondary channel base: 0x%08x\n", ata_channels[ATA_SECONDARY].base);
  /* ata device stuff */
  ata_devices[num_ata] = device_struct_alloc();
  ata_devices[num_ata]->removable   = false;
  ata_devices[num_ata]->major       = DISKDEV_MAJOR;
  ata_devices[num_ata]->minors      = num_ata;
  ata_devices[num_ata]->priv_data   = drv;
  //partitions = ata_count_partitions();
  rc = register_blkdev(DISKDEV_MAJOR, "hda", ata_devices[num_ata], &ata_fops);
  if(IS_ERR(rc))
  {
    printk("Failed to register hard-disk: %d\n", rc);
    return rc;
  }
  num_ata++;
  debug("ATA: Installing interrupt handlers..\n");
  interrupt_install(ata_primary_irq, 14);
  interrupt_install(ata_secondary_irq, 15);
  return 0;
}

static void ata_do_400ns_delay(void)
{
  for(int i = 0; i < 4; i++);
}

/* im using one of my old ATA PIO drivers as reference */

static void ata_prepare_pio(int disk, int lba)
{
	outb(ata_channels[disk].base + 1, 0x00); // <- slows down the CPU a lot
	outb(ata_channels[disk].base + 2, 0x01); /* read only one sector */
  /* send lba 'n stuff */
	outb(ata_channels[disk].base + 3, (uint8_t)lba);
	outb(ata_channels[disk].base + 4, (uint8_t)(lba >> 8));
	outb(ata_channels[disk].base + 5, (uint8_t)(lba >> 16));
	outb(ata_channels[disk].base + 6, 0xe0 | (disk << 4) | ((lba >> 24) & 0x0f));
  outb(ata_channels[disk].base + 7, ATA_CMD_READ_PIO);
  ATA_WAIT_FOR_STATUS(ATA_STATUS_BUSY);
}

static void ata_write_single_sector_pio(int disk, uint8_t * buf, int lba)
{
  ata_prepare_pio(disk, lba);
	outb(ata_channels[disk].base + 7, ATA_CMD_WRITE_PIO);
  for(int i = 0; i < 256; i++)
  {
    uint16_t tmp = (uint16_t)(buf[i * 2 + 1] << 8 | buf[i * 2]);
    outb(ata_channels[disk].base, tmp);
  }
}

static void ata_read_single_sector_pio(int disk, uint8_t * buf, int lba)
{
  ata_prepare_pio(disk, lba);
	for(int i = 0; i < 256; i++)
	{
		uint16_t tmp = inw(ata_channels[disk].base);
		buf[i * 2] = (uint8_t)tmp;
		buf[i * 2 + 1] = (uint8_t)(tmp >> 8);
	}
  ata_do_400ns_delay();
}

static void ata_read_sectors_pio(int disk, uint8_t * buf, int lba, size_t sectors)
{
	for(int i = 0; i < sectors; i++)
	{
		ata_read_single_sector_pio(disk, buf + i * 512, lba + i);
	}
}

static void ata_write_sectors_pio(int disk, uint8_t * buf, int lba, size_t sectors)
{
	for(int i = 0; i < sectors; i++)
	{
		ata_write_single_sector_pio(disk, buf + i * 512, lba + i);
	}
}

int ata_find_first_partition(void)
{
	ata_prepare_pio(0, 0);
	uint16_t pos = 0;
	for(int i = 0; i < 256; i++)
	{
		uint16_t tmpword = inw(ata_channels[ATA_PRIMARY].base);
		if(i == 227)
		{
			pos = tmpword;
		}
	}
	return pos;
}

int ata_dev_read(struct file * file, void * buf, size_t sz)
{
  ata_read_sectors_pio(0, (uint8_t *)buf, lseek, sz);
  return 0;
}

int ata_dev_write(struct file * file, void * buf, size_t sz)
{
  ata_write_sectors_pio(0, (uint8_t *)buf, lseek, sz);
  return 0;
}

int ata_dev_open(struct file * file, const char * unused)
{
  return 0;
}

void ata_dev_close(struct file * file)
{
  return;
}

int ata_dev_lseek(struct file * file, size_t offset, int whence)
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

struct file_operations ata_fops = {
  ata_dev_lseek,        /* lseek */
  ata_dev_read,         /* read */
  ata_dev_write,        /* write */
  NULL,                 /* readdir */
  NULL,                 /* ioctl */
  ata_dev_open,         /* open */
  ata_dev_close,        /* close */
};
