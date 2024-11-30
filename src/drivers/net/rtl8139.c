#include <stdint.h>
#include <catk/core.h>
#include <catk/device.h>
#include <catk/pci.h>
#include <catk/errno.h>
#include <catk/printk.h>
#include <catk/debug.h>
#include <catk/mem.h>
#include <catk/io.h>
#include <lib/common.h>

/* port offsets */
#define RTL8139_RBSTART_REG 0x30
#define RTL8139_CMD_REG     0x37
#define RTL8139_IMR_REG     0x3c
#define RTL8139_ISR_REG     0x3e
#define RTL8139_CONFIG1_REG 0x52

/* command register bit thingys */
#define RTL8139_CR_BUFE BIT(0)
#define RTL8139_CR_TE   BIT(2)
#define RTL8139_CR_RE   BIT(3)
#define RTL8139_CR_RST  BIT(4)

static struct pci_device * pci_rtl8139;
static uint32_t bar0;

static uint8_t * rx_buffer = NULL;

static const struct pci_ident rtl8139_ids[] = 
{
  PCI_IDENT(REALTEK, 0x8139),
  PCI_IDENT(0xffff, 0xffff)
};

static int rtl8139_probe(struct pci_device * pd) {
  return (pd->subclass == 0 && pd->class == 2);
}

static int rtl8139_attach(struct pci_device * dev) {
  int rc;
  debug("Allocating memory for RX buffer..\n");
  rx_buffer = (uint8_t *)malloc(8208);
  if(rx_buffer == NULL) {
    return -ENOMEM;
  }
  rc = rtl8139_probe(dev);
  if(rc == false) {
    printk("Incorrect classes\n");
    return -EINVAL;
  }
  bar0 = (pci_get_bar(dev->bus, dev->slot, dev->functions, 0) & ~3);
  pci_rtl8139 = dev;
  pci_enable_busmaster(dev);
  return 0;
}

static void rtl8139_init(void) {
  debug("Powering on RTL8139...\n");
  /* power on */
  outb(bar0 + RTL8139_CONFIG1_REG, 0);
  /* clear rx and tx buffers */
  debug("Clearing RX & TX buffers..\n");
  outb(bar0 + RTL8139_CMD_REG, 0x10); // This triggers a little software reset
  while(inb(bar0 + RTL8139_CMD_REG) & RTL8139_CR_RST); // now we wait until the reset is complete..
  /* initialize receive buffer */
  outl(bar0 + RTL8139_RBSTART_REG, (uintptr_t)rx_buffer);
  /* set imr and isr */
  outw(bar0 + RTL8139_IMR_REG, 0x0005);
  outl(bar0 + 0x44, 0xf | (1 << 7)); // (1 << 7) is the WRAP bit, 0xf is AB+AM+APM+AAP
  outb(bar0 + RTL8139_CMD_REG, 0x0c); // Sets the RE and TE bits high
  //interrupt_install();
}

struct pci_driver rtl8139_driver = {
  .name = "Realtek 8139 Ethernet",
  .ident = (void *)&rtl8139_ids,
  .attach_driver = rtl8139_attach,
  .init_driver   = rtl8139_init
};
