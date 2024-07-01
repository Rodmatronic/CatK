#include <stdint.h>
#include <catk/core.h>
#include <catk/device.h>
#include <catk/errno.h>
#include <catk/printk.h>
#include <catk/mem.h>
#include <catk/io.h>
#include <lib/common.h>

#include "ide.h"

static int num_ata = 0;
static struct device * ata_devices[4];

struct file_operations ata_fops;

int ata_finalize_init(struct ide_drive * drv, const struct pci_device * dev)
{
  int rc;
  ata_devices[num_ata]->removable = false;
  ata_devices[num_ata]->minors = num_ata;
  ata_devices[num_ata]->priv_data = drv;
  rc = register_blkdev(DISKDEV_MAJOR, "hda", ata_devices[num_ata], &ata_fops);
  if(IS_ERR(rc))
  {
    printk("Failed to register hard-disk: %d\n", rc);
    return rc;
  }
  num_ata++;
  return 0;
}

/* im using one of my old ATA PIO drivers as reference */

struct file_operations ata_fops = {
  NULL,   /* read */
  NULL,   /* write */
  NULL,   /* readdir */
  NULL,   /* ioctl */
  NULL,   /* open */
  NULL,   /* close */
};
