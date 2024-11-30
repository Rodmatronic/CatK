#ifndef __PCI_H
#define __PCI_H

#include <stdint.h>
#include <catk/device.h>

// helper macros
#define PCI_IDENT(ven, dev) \
  {ven, dev}

#define INTEL 0x8086
#define REALTEK 0x10ec

struct pci_device;

struct pci_ident
{
  uint16_t ven;
  uint16_t dev;
};

struct pci_driver
{
  char * name;
  struct pci_ident * ident;
  int (*attach_driver)(struct pci_device *);
  void (*init_driver)(void);
};

struct pci_device
{
  struct pci_ident ident;
  uint8_t prog_if;
  uint8_t subclass;
  uint8_t class;
  uint8_t bus, slot, functions;
  struct pci_driver * driver;
  struct device * dev;
};

/* built-in drivers */
extern struct pci_driver ide_driver;
extern struct pci_driver ne2000_driver;
extern struct pci_driver sata_driver;

void pci_enable_busmaster(struct pci_device * dev);
uint32_t pci_get_bar(uint8_t bus, uint8_t slot, uint8_t func, uint8_t bar);
void pci_init(void);

#endif
