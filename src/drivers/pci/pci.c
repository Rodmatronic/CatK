#include <catk/io.h>
#include <catk/pci.h>
#include <catk/printk.h>
#include <catk/debug.h>
#include <stdint.h>

#define PCI_CONFIG_ADDR 0xcf8
#define PCI_CONFIG_DATA 0xcfc

#define PCI_VENDOR_INVALID 0xffff

int num_pci = 0;

static struct pci_device devices[32];

/* these are built in the kernel */
static struct pci_driver * drivers[] =
{
  &ide_driver,
  NULL
};

static uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
  uint32_t address;
  uint32_t lbus  = (uint32_t)bus;
  uint32_t lslot = (uint32_t)slot;
  uint32_t lfunc = (uint32_t)func;
  uint16_t tmp;

  address = (uint32_t)((lbus << 16) | (lslot << 11) |
    (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));

  outl(PCI_CONFIG_ADDR, address);
  tmp = (uint16_t)((inl(PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xffff);
  return tmp;
}

static void pci_config_write(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t data)
{
  uint32_t address;
  uint32_t lbus  = (uint32_t)bus;
  uint32_t lslot = (uint32_t)slot;
  uint32_t lfunc = (uint32_t)func;
  uint16_t tmp = 0;

  address = (uint32_t)((lbus << 16) | (lslot << 11) |
    (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));

  outl(PCI_CONFIG_ADDR, address);
  outl(PCI_CONFIG_DATA, data);
}

void pci_enable_busmaster(struct pci_device * dev)
{
  uint16_t cmd_reg = pci_config_read_word(dev->bus, dev->slot, dev->functions, 4);
  uint16_t status = pci_config_read_word(dev->bus, dev->slot, dev->functions, 6);
  cmd_reg |= BIT(2);
  pci_config_write(dev->bus, dev->slot, dev->functions, 4, (uint32_t)status << 16 | (uint32_t)cmd_reg);
}

static uint32_t pci_read_dword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
  uint32_t low = pci_config_read_word(bus, slot, func, offset);
  uint32_t high = pci_config_read_word(bus, slot, func, offset + 2);

  return (high << 16) | low;
}

static uint16_t pci_get_vendor(uint8_t bus, uint8_t slot, uint8_t func)
{
  uint32_t vendor = pci_config_read_word(bus, slot, func, 0);
  if (vendor == PCI_VENDOR_INVALID)
    return 0;
  return vendor;
}

static uint16_t pci_get_device(uint8_t bus, uint8_t slot, uint8_t func)
{
  uint32_t device = pci_config_read_word(bus, slot, func, 2);
  return device;
}

static uint8_t pci_get_subclass(uint8_t bus, uint8_t slot, uint8_t func)
{
  uint32_t subclass = pci_config_read_word(bus, slot, func, 10);
  return (subclass & ~0xff00);
}

static uint8_t pci_get_class(uint8_t bus, uint8_t slot, uint8_t func)
{
  uint32_t class = pci_config_read_word(bus, slot, func, 10);
  return (class & ~0x00ff) >> 8;
}

static uint8_t pci_get_progif(uint8_t bus, uint8_t slot, uint8_t func)
{
  uint32_t progif = pci_config_read_word(bus, slot, func, 8);
  return (progif & ~0x00ff) >> 8;
}

static uint8_t pci_get_revision(uint8_t bus, uint8_t slot, uint8_t func)
{
  uint32_t revision = pci_config_read_word(bus, slot, func, 8);
  return (revision & ~0xff00);
}

static uint8_t pci_get_header(uint8_t bus, uint8_t slot, uint8_t func)
{
  uint32_t header = pci_config_read_word(bus, slot, func, 14);
  return (header & 0xff);
}

uint32_t pci_get_bar(uint8_t bus, uint8_t slot, uint8_t func, uint8_t bar)
{
  uint8_t offset = 0x10 + (bar * 4);
  uint32_t rc = pci_read_dword(bus, slot, func, offset);
  return rc;
}

static bool pci_is_invalid(uint8_t bus, uint8_t slot, uint8_t func)
{
  return (pci_get_vendor(bus, slot, func) == 0);
}

static void pci_register_device(uint8_t bus, uint8_t slot, uint8_t func)
{
  devices[num_pci].ident.ven = pci_get_vendor(bus, slot, func);
  devices[num_pci].ident.dev = pci_get_device(bus, slot, func);
  devices[num_pci].prog_if = pci_get_progif(bus, slot, func);
  devices[num_pci].subclass = pci_get_subclass(bus, slot, func);
  devices[num_pci].class = pci_get_class(bus, slot, func);
  devices[num_pci].bus = bus;
  devices[num_pci].slot = slot;
  devices[num_pci].functions = func;
  debug("  PCI = %04x : %04x\n", __FUNCTION__, 
    devices[num_pci].ident.ven, devices[num_pci].ident.dev);

  // Print a message based on the type of PCI device
  switch (devices[num_pci].class) {
    case 0x01: // Mass Storage Controllers
      switch (devices[num_pci].subclass) {
        case 0x01: printk("  class 0x%02x subclass 0x%02x - SATA Controller found\n", devices[num_pci].class, devices[num_pci].subclass); break;
        case 0x00: printk("  class 0x%02x subclass 0x%02x - IDE Controller found\n", devices[num_pci].class, devices[num_pci].subclass); break;
        case 0x06: printk("  class 0x%02x subclass 0x%02x - SCSI Controller found\n", devices[num_pci].class, devices[num_pci].subclass); break;
        default: printk("  class 0x%02x subclass 0x%02x - Mass Storage Controller found\n", devices[num_pci].class, devices[num_pci].subclass); break;
      }
      break;
    case 0x03: // Display Controllers
      printk("  class 0x%02x subclass NONE - Graphics Card found\n", devices[num_pci].subclass);
      break;
    case 0x04: // Multimedia Devices
      switch (devices[num_pci].subclass) {
        case 0x01: printk("  class 0x%02x subclass 0x%02x - Audio Controller found\n", devices[num_pci].class, devices[num_pci].subclass); break;
        case 0x02: printk("  class 0x%02x subclass 0x%02x - Video Controller found\n", devices[num_pci].class, devices[num_pci].subclass); break;
        default: printk("  class 0x%02x subclass 0x%02x - Multimedia Device found\n", devices[num_pci].class, devices[num_pci].subclass); break;
      }
      break;
    case 0x05: // Memory Controllers
      printk("  class 0x%02x subclass NONE - Memory Controller found\n", devices[num_pci].class);
      break;
    case 0x06: // Bridge Devices
      switch (devices[num_pci].subclass) {
        case 0x00: printk("  class 0x%02x subclass 0x%02x - PCI-to-PCI Bridge found\n"); break;
        case 0x01: printk("  class 0x%02x subclass 0x%02x - ISA Bridge found\n", devices[num_pci].class, devices[num_pci].subclass); break;
        default: printk("  class 0x%02x subclass 0x%02x - Bridge Device found\n", devices[num_pci].class, devices[num_pci].subclass); break;
      }
      break;
    case 0x02: // Network Controllers
      if (devices[num_pci].subclass == 0x00) {
        printk("  class 0x%02x subclass 0x%02x - Ethernet Controller found\n", devices[num_pci].class, devices[num_pci].subclass);
      } else {
        printk("  class 0x%02x subclass 0x%02x - Network Controller found\n", devices[num_pci].class, devices[num_pci].subclass);
      }
      break;
    default:
      printk("  class 0x%02x subclass NONE - Unknown PCI device found\n", devices[num_pci].class);
      break;
  }

  num_pci++;
}

static void pci_enumerate(void)
{
  debug("pci_register_device:\n        VEN      DEV\n");
  for (int bus = 0; bus < 256; bus++)
  {
    for (int slot = 0; slot < 32; slot++)
    {
      for(int func = 0; func < 8; func++)
      {
        if(pci_is_invalid(bus, slot, func))
          continue;
        pci_register_device(bus, slot, func);
      }
    }
  }
}

static inline int pci_compare(struct pci_ident ident1, struct pci_ident * ident2)
{
  debug("  Comparing = %04x : %04x to %04x:%04x\n", ident1.ven, ident1.dev, ident2->ven, ident2->dev);
  return (ident1.ven == ident2->ven && ident1.dev == ident2->dev);
}

static int pci_driver_attach(struct pci_device * dev, struct pci_driver * drv)
{
  int rc;
  printk("Attaching driver \"%s\"...\n", drv->name);
  dev->driver = drv;
  rc = dev->driver->attach_driver(dev);
  debug("  drv = %d\n  rc = %d\n", drv, rc);
  return rc;
}

static inline void pci_driver_init(struct pci_device * dev)
{
  dev->driver->init_driver();
}

static void pci_drivers_find(void) /* i hate this coding this damn function */
{
  printk("Finding PCI drivers..\n");
  int rc;
  debug("pci_compare:\n");
  for(int i = 0; i < num_pci; i++)
  {
    struct pci_device d = devices[i];
    for(int j = 0; drivers[j]; j++)
    {
      int k = 0;
      struct pci_ident * ident = &drivers[j]->ident[k];
      while(ident->ven != PCI_VENDOR_INVALID)
      {
        ident = &drivers[j]->ident[k++];
        if(pci_compare(d.ident, ident))
        {
          rc = pci_driver_attach(&d, drivers[j]);
          if(rc < 0)
          {
            printk("Driver attachment failed: %d\n", rc);
            continue;
          }
          printk("Initializing driver \"%s\"...\n", d.driver->name);
          pci_driver_init(&d);
        }
      }
    }
  }
}

void pci_init(void)
{
  memset(devices, 0, sizeof(struct pci_device) * 32);
  pci_enumerate();
  pci_drivers_find();
}
