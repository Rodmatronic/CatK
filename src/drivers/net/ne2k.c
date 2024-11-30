#include <stdint.h>
#include <catk/core.h>
#include <catk/device.h>
#include <catk/pci.h>
#include <catk/errno.h>
#include <catk/printk.h>
#include <catk/mem.h>
#include <catk/io.h>
#include <lib/common.h>

#define NE2K_RESET 0x1f

static struct pci_device * pci_ne2k;
static uint32_t bar0;

static const struct pci_ident ne2000_ids[] = 
{
  PCI_IDENT(REALTEK, 0x8029),
  PCI_IDENT(0xffff, 0xffff)
};

static int ne2000_probe(struct pci_device * pd) {
  return (pd->subclass == 0 && pd->class == 2);
}

static int ne2000_attach(struct pci_device * dev) {
  int rc;
  rc = ne2000_probe(dev);
  if(rc == false) {
    printk("Incorrect classes\n");
    return -EINVAL;
  }
  bar0 = (pci_get_bar(dev->bus, dev->slot, dev->functions, 0) & ~3);
  pci_ne2k = dev;
  return 0;
}

static void ne2000_write(uint8_t page, uint8_t reg, uint8_t value) {
  uint8_t command = inb(bar0 + 0x00);
  command = (command & 0x3F) | (page << 6);
  outb(command, bar0 + 0x00);
  outb(value, bar0 + reg);
}

static void ne2000_init(void) {
  outb(bar0 + 0x1F, inb(bar0 + 0x1F));  // write the value of RESET into the RESET register
  while ((inb(bar0 + 0x07) & 0x80) == 0);      // wait for the RESET to complete
  outb(bar0 + 0x07, 0xFF);                     // mask interrupts

  uint8_t prom[32];
  outb(bar0, (1 << 5) | 1);	// page 0, no DMA, stop
  outb(bar0 + 0x0E, 0x49);		// set word-wide access
  outb(bar0 + 0x0A, 0);		// clear the count regs
  outb(bar0 + 0x0B, 0);
  outb(bar0 + 0x0F, 0);		// mask completion IRQ
  outb(bar0 + 0x07, 0xFF);
  outb(bar0 + 0x0C, 0x20);		// set to monitor
  outb(bar0 + 0x0D, 0x02);		// and loopback mode.
  outb(bar0 + 0x0A, 32);		// reading 32 bytes
  outb(bar0 + 0x0B, 0);		// count high
  outb(bar0 + 0x08, 0);		// start DMA at 0
  outb(bar0 + 0x09, 0);		// start DMA high
  outb(bar0, 0x0A);		// start the read

  for (int i = 0; i < 32; i++)
  {
    prom[i] = inb(bar0 + 0x10);
  };

  for (int i = 0; i < 6; i++)
  {
    ne2000_write(1, 0x01 + i, prom[i]);
  };
  printk("MAC Address: %02x:%02x:%02x:%02x:%02x:%02x\n", prom[0], prom[1], prom[2], prom[3], prom[4], prom[5], prom[6]);
}

struct pci_driver ne2000_driver = {
  .name = "NE2000 Compatible Ethernet",
  .ident = (void *)&ne2000_ids,
  .attach_driver = ne2000_attach,
  .init_driver   = ne2000_init
};
