#include <stdint.h>
#include <catk/core.h>
#include <catk/device.h>
#include <catk/pci.h>
#include <catk/errno.h>
#include <catk/printk.h>
#include <catk/mem.h>
#include <catk/io.h>
#include <lib/common.h>

#include "ide.h"

static uint32_t bar0, bar1, bar2, bar3, bar4;

static struct pci_device * pci_ide;

static struct ide_drive drives[4];

static int num_drives = 0;

// linux-like pci driver setup
static const struct pci_ident ata_ids[] = 
{
  PCI_IDENT(INTEL, 0x7010), // 82371SB PIIX3 IDE [Natoma/Triton II]
  PCI_IDENT(INTEL, 0x7111), // 82371AB/EB/MB PIIX4 IDE
  PCI_IDENT(0xffff, 0xffff)
};

static int ide_probe(struct pci_device * pd)
{
  return (pd->subclass == 1 && pd->class == 1);
}

static int ide_driver_attach(struct pci_device * dev)
{
  int rc = 0;
  rc = ide_probe(dev);
  if(!rc)
  {
    printk("Incorrect classes\n");
    return -EINVAL;
  }

  // If bit 0 and 2 are not set in PROG_IF, this means that the drive is in compatibility mode
  // Because of that, we can use the default bar values.
  if(dev->prog_if & ATA_PRIM_LEGACY)
    printk("Drive is in compatibility mode. Setting defaults...\n");
  bar0 = (dev->prog_if & ATA_PRIM_LEGACY) ? (pci_get_bar(dev->bus, dev->slot, dev->functions, 0) & ~3) : ATA_BUS1_PRIMARY_IO_PORT;
  bar1 = (dev->prog_if & ATA_PRIM_LEGACY) ? (pci_get_bar(dev->bus, dev->slot, dev->functions, 1) & ~3) : ATA_BUS1_PRIMARY_CTRL_REG;
  bar2 = (dev->prog_if & ATA_PRIM_LEGACY) ? (pci_get_bar(dev->bus, dev->slot, dev->functions, 2) & ~3) : ATA_BUS1_SECOND_IO_PORT;
  bar3 = (dev->prog_if & ATA_PRIM_LEGACY) ? (pci_get_bar(dev->bus, dev->slot, dev->functions, 3) & ~3) : ATA_BUS1_SECONDARY_CTRL_REG;
  bar4 = (pci_get_bar(dev->bus, dev->slot, dev->functions, 4) & ~3);
  pci_enable_busmaster(dev);
  pci_ide = dev;
  return 0;
}

static void ide_send_cmd(uint32_t bar, uint8_t cmd)
{
  outb(bar + 7, cmd);
}

static uint8_t ide_read_status(uint32_t bar)
{
  return inb(bar + 7);
}

static void ide_select_drive(uint32_t bar, uint8_t opt)
{
  outb(bar + 6, opt);
}

static void ide_reset(uint32_t bar)
{
  // make sure it gets to 5
  for(int i = 2; i < 6; i++)
  {
    outw(bar + i, 0);
  }
}

static uint8_t ide_read_err(uint32_t bar)
{
  return inb(bar + 1);
}

static void ide_init()
{
  int rc, retry = 0;
retry:;
  uint32_t bar = retry ? bar2 : bar0;
  ide_send_cmd(bar, ATA_CMD_PACKET);
  ide_select_drive(bar, retry ? ATA_SLAVE : ATA_MASTER); // master drive
  // reset sectorcount, and other things
  ide_reset(bar);
  ide_send_cmd(bar, ATA_CMD_IDENTIFY);
  rc = ide_read_status(bar);
  if(!rc)
  {
    printk("Failed to initialize IDE controller.\n");
    if(retry == 1)
      return;
    retry = 1;
    goto retry;
  }
  while((ide_read_status(bar) & ATA_STATUS_BUSY));
loop:;
  rc = ide_read_status(bar);
  if(rc & ATA_STATUS_ERR)
  {
    int rc2 = ide_read_err(bar);
    if(rc2 != 4)
    {
      printk("Drive is in error mode. Code: 0x%x\n", rc2);
      return;
    }
    else
    {
      uint8_t sig1 = inb(bar + 4);
      uint8_t sig2 = inb(bar + 5);
      printk("This drive might be different...\n");
      if(sig1 == 0x14 && sig2 == 0xeb)
      {
        printk("Detected ATAPI drive.\n");
        return;
      }
      else
      {
        if(sig1 == 0x3c && sig2 == 0xc3)
        {
          printk("Detected SATA drive.\n");
          return;
        }
        else
        {
          printk("Unknown drive signature: 0x%02x, 0x%02x\n", sig1, sig2);
        }
      }
      return;
    }
  }
  if(!(rc & ATA_STATUS_DRQ))
    goto loop;
  void * init_data = (void *)malloc(512);
  for(int i = 0; i < 256; i++)
    *(uint16_t *)(init_data + i * 2) = inw(bar);
  // get drive info
  drives[num_drives].used = true;
  drives[num_drives].signature = *((uint16_t *)(init_data + ATA_IDENT_DEVICETYPE));
  drives[num_drives].features = *((uint16_t *)(init_data + ATA_IDENT_CAPABILITIES));
  drives[num_drives].commands = *((uint32_t *)(init_data + ATA_IDENT_COMMANDSETS));
  if(drives[num_drives].commands & ATA_48BIT_ADDR)
  {
    drives[num_drives].size = *((uint32_t *)(init_data + ATA_IDENT_MAX_LBA_EXT));
  }
  else
  {
    drives[num_drives].size = *((uint32_t *)(init_data + ATA_IDENT_MAX_LBA));
  }

  for(int k = 0; k < 40; k += 2) 
  {
    drives[num_drives].name[k] = *(uint16_t *)(init_data + ATA_IDENT_MODEL + k + 1);
    drives[num_drives].name[k + 1] = *(uint16_t *)(init_data + ATA_IDENT_MODEL + k);
  }
  rc = ata_finalize_init(&drives[num_drives], bar0, bar1, bar2, bar3, bar4);
  if(!IS_ERR(rc))
    printk("Successfully initialized drive: %s\n", drives[num_drives].name);
  num_drives++;
  free(init_data);
}

static void ide_driver_init(void)
{
  ide_init();
}

struct pci_driver ide_driver = {
  .name = "IDE controller",
  .ident = (void *)&ata_ids,
  .attach_driver = ide_driver_attach,
  .init_driver = ide_driver_init
};
