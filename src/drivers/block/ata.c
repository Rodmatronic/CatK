#include <stdint.h>
#include <catk/mem.h>
#include <catk/core.h>
#include <catk/io.h>
#include <catk/printk.h>
#include <catk/debug.h>
#include <catk/device.h>
#include <catk/errno.h>
#include <lib/common.h>

#define ATA_BUS1_PRIMARY_IO_PORT    0x1f0
#define ATA_BUS1_SECOND_IO_PORT     0x170

#define ATA_BUS2_PRIMARY_IO_PORT    0x1e8
#define ATA_BUS2_SECOND_IO_PORT     0x168

#define ATA_BUS1_PRIMARY_CTRL_REG   0x3f6
#define ATA_BUS1_SECONDARY_CTRL_REG 0x376

#define ATA_BUS2_PRIMARY_CTRL_REG   0x3e6
#define ATA_BUS2_SECOND_CTRL_REG    0x366


/*
* Status Register Layout
* Bits:
*   0    1     2    3    4   5    6    7
* [ERR][IDX][CORR][DRQ][SRV][DF][RDY][BSY] <- not sure if the eigth bit is zero or not
* 
*/

#define ATA_STATUS_ERR  BIT(0)
#define ATA_STATUS_DRQ  BIT(3)
#define ATA_STATUS_SRV  BIT(4)
#define ATA_STATUS_DF   BIT(5)
#define ATA_STATUS_RDY  BIT(6)
#define ATA_STATUS_BUSY BIT(7)

// ATA commands

#define ATA_CMD_READ_PIO          0x20
#define ATA_CMD_READ_PIO_EXT      0x24
#define ATA_CMD_READ_DMA          0xc8
#define ATA_CMD_READ_DMA_EXT      0x25
#define ATA_CMD_WRITE_PIO         0x30
#define ATA_CMD_WRITE_PIO_EXT     0x34
#define ATA_CMD_WRITE_DMA         0xca
#define ATA_CMD_WRITE_DMA_EXT     0x35
#define ATA_CMD_CACHE_FLUSH       0xe7
#define ATA_CMD_CACHE_FLUSH_EXT   0xea
#define ATA_CMD_PACKET            0xa0
#define ATA_CMD_IDENTIFY_PACKET   0xa1
#define ATA_CMD_IDENTIFY          0xec

// ATAPI commands

#define ATAPI_CMD_READ            0xa8
#define ATAPI_CMD_EJECT           0x1b

// Identification space

#define ATA_IDENT_DEVICETYPE   0
#define ATA_IDENT_CYLINDERS    2
#define ATA_IDENT_HEADS        6
#define ATA_IDENT_SECTORS      12
#define ATA_IDENT_SERIAL       20
#define ATA_IDENT_MODEL        54
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID   106
#define ATA_IDENT_MAX_LBA      120
#define ATA_IDENT_COMMANDSETS  164
#define ATA_IDENT_MAX_LBA_EXT  200

#define ATA_PRIM_LEGACY BIT(0)
#define ATA_SEC_LEGACY  BIT(2)

// Command set stuff
#define ATA_48BIT_ADDR BIT(26)

// Drive select

#define ATA_MASTER 0xa0
#define ATA_SLAVE 0xb0

struct ide_drive
{
  uint8_t used;
  uint8_t drive;
  uint16_t type;
  uint16_t signature;
  uint16_t features;
  uint32_t commands;
  uint32_t size;
  uint8_t name[41];
};

struct ide_drive drives[4];

static int num_drives = 0;

uint8_t ata_status_read(uint16_t bus);
void ata_write_features(uint16_t bus, uint16_t data);
void ata_send_command(uint16_t bus, uint8_t cmd);

// basic macros
#define ATA_STATE(x, y) \
  (ata_status_read(x) & y)

#define ATA_WAIT_NOT_BUSY(x) \
  while(ATA_STATE(x, ATA_STATUS_BUSY));

uint8_t ata_status_read(uint16_t bus);

uint16_t ata_data[512];

static void ata_reset_sectorcount(uint16_t bus)
{
  debug("[ata_dev] resetting %s ata sectorcount...\n", bus ? "primary" : "secondary");
  outb(bus + 2, 0);
  outb(bus + 3, 0);
  outb(bus + 4, 0);
  outb(bus + 5, 0);
}

void ata_send_command(uint16_t bus, uint8_t cmd)
{
  outb(bus + 7, cmd);
}

uint8_t ata_status_read(uint16_t bus)
{
  return inb(bus + 7);
}

uint16_t ata_read_data(uint16_t bus)
{
  return inw(bus);
}

uint16_t ata_get_error(uint16_t bus)
{
  return inb(bus + 1);
}

void ata_write_features(uint16_t bus, uint16_t data)
{
  outb(bus + 1, data);
}

static int ata_identify(uint16_t bus)
{
  debug("Identifying ATA drives...\n");
  int rc;
  // reset sectorcount
  ata_reset_sectorcount(bus); 
  ata_send_command(bus, ATA_CMD_IDENTIFY);
  rc = ata_status_read(bus);
  if(!rc)
  {
    rc = -ENODEV;
    debug("[ata_dev] drive does not exist: %d\n", rc);
    return rc;
  }
  debug("[ata_dev] ata identify returned 0x%x\n", rc);
  ATA_WAIT_NOT_BUSY(bus);
  debug("[ata_dev] polling ata status...\n");

ata_poll:
  if(ATA_STATE(bus, ATA_STATUS_ERR))
  {
    rc = -EIO;
    printk("%s ata is in error mode. 0x%x\n", bus ? "primary" : "secondary", ata_get_error(bus));
    debug("[ata_dev] cannot initialize %s ata. %d", bus ? "primary" : "secondary", rc);
    return -EIO;
  }

  if(!(ATA_STATE(bus, ATA_STATUS_DRQ)))
    goto ata_poll;

  debug("[ata_dev] storing ata data...\n");

  void * init_data = (void *)malloc(512);
  for(int i = 0; i < 256; i++)
    *(uint16_t *)(init_data + i * 2) = inw(bus);
  // get drive info
  drives[num_drives].used = true;
  drives[num_drives].drive = ATA_MASTER;
  drives[num_drives].signature = *((uint16_t *)(init_data + ATA_IDENT_DEVICETYPE));
  drives[num_drives].features = *((uint16_t *)(init_data + ATA_IDENT_CAPABILITIES));
  drives[num_drives].commands = *((uint32_t *)(init_data + ATA_IDENT_COMMANDSETS));

  if(drives[num_drives].commands & ATA_48BIT_ADDR)
    drives[num_drives].size = *((uint32_t *)(init_data + ATA_IDENT_MAX_LBA_EXT));
  else
    drives[num_drives].size = *((uint32_t *)(init_data + ATA_IDENT_MAX_LBA));

  for(int k = 0; k < 40; k += 2) 
  {
    drives[num_drives].name[k] = *(uint16_t *)(init_data + ATA_IDENT_MODEL + k + 1);
    drives[num_drives].name[k + 1] = *(uint16_t *)(init_data + ATA_IDENT_MODEL + k);
  }
  printk("Initialized %s ATA device %s\n", bus ? "Primary": "Secondary", drives[num_drives].name);
  debug("[ata_dev] %s ata device (%s) initialized.\n", bus ? "primary": "secondary", drives[num_drives].name);
  free(init_data);
  return 0;
}

int ata_device_probe(void)
{ 
  // by default, use the primary ata bus
  return ata_identify(ATA_BUS1_PRIMARY_IO_PORT);
}
