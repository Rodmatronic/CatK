#define REG_CYL_LO 4
#define REG_CYL_HI 5
#define REG_DEVSEL 6

#define ATADEV_UNKNOWN 0
#define ATADEV_PATA 1
#define ATADEV_PATAPI 2
#define ATADEV_SATA 3
#define ATADEV_SATAPI 4

struct DEVICE {
    unsigned short base;
    unsigned short dev_ctl;
};

void listdrivebits()
{
    struct DEVICE ctrl;
    ctrl.base = 0x1F0;
    ctrl.dev_ctl = 0x3F6;

    for (int slavebit = 0; slavebit < 2; slavebit++) {
        int devtype = detect_devtype(slavebit, &ctrl);
        if (devtype != ATADEV_UNKNOWN) {
            printf("Drive %d is available, type: %d\n", slavebit, devtype);
        }
    }   
}

void ata_soft_reset(unsigned short dev_ctl) {
    outportb(dev_ctl, 4);
    inportb(dev_ctl);
    outportb(dev_ctl, 0);
    inportb(dev_ctl);
}

int detect_devtype(int slavebit, struct DEVICE *ctrl) {
    ata_soft_reset(ctrl->dev_ctl);
    outportb(ctrl->base + REG_DEVSEL, 0xA0 | (slavebit << 4));
    inportb(ctrl->dev_ctl);

    // Wait 400ns for drive select to work
    for (int i = 0; i < 4; i++) {
        inportb(ctrl->dev_ctl);
    }

    unsigned cl = inportb(ctrl->base + REG_CYL_LO);
    unsigned ch = inportb(ctrl->base + REG_CYL_HI);

    if (cl == 0x14 && ch == 0xEB) {
        return ATADEV_PATAPI;
    } else if (cl == 0x69 && ch == 0x96) {
        return ATADEV_SATAPI;
    } else if (cl == 0 && ch == 0) {
        return ATADEV_PATA;
    } else if (cl == 0x3c && ch == 0xc3) {
        return ATADEV_SATA;
    }
    return ATADEV_UNKNOWN;
}

struct bootsector {
    char jump[3];
    char fsid[8];
    char pad[53];
    uint64 unk1; // Rename to a unique name, e.g., unk1
    uint64 fsSizeInSectors;
    uint32 sectorsToStartOfFat;
    uint32 sectorsUsedForFat;
    uint32 sectorStartOfFirstCluster;
    uint32 clusterCountInFs;
    uint32 clusterForRootDirectory;
    uint32 unk2; // Rename to a unique name, e.g., unk2
    uint8 logBytesPerSector, logSectorsPerCluster, fatCount, driveId;
    uint8 percentInUse;
    uint8 unk3[7]; // Rename to a unique name, e.g., unk3
    char bootcode[390];
    char bootsign[2];
};

uint32 BootChecksum (uchar * Sectors, short BytesPerSector)
{
  /*
    UCHAR * Sectors, above, points to an in-memory copy of the 11 sectors
    That is, 0 - 10 for the Main Boot Region, or 12 - 22 for the Backup Boot Region
  */
 
  uint32 NumberOfBytes = (uint32)BytesPerSector * 11;
  uint32 Checksum = 0;
  uint32 Index;
 
  for (Index = 0; Index < NumberOfBytes; Index++)
  {
    if ((Index == 106) || (Index == 107) || (Index == 112))
    {
      continue;
    }
    Checksum = ((Checksum&1) ? 0x80000000 : 0) + (Checksum>>1) + (uint32)Sectors[Index];
  }
  return Checksum;
}

struct {
    uint8 entrytype;
    uint8 entrycount;
    char pad[2];
    uint32 flags;
    uint32 creation;
    uint32 modification;
    uint32 access;
    char reserved[12];
} fileEntry = { 0x85, 0, {0, 0}, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };

struct {
    uint8 entryType;
    uint8 flags;
    uint8 unk;
    uint8 filenameLengthInBytes;
    uint64 filesize;
    uint32 reserved;
    uint32 startCluster;
    uint64 filesize2;
} fileInfoEntry = { 0xC0, 0, 0, 0, 0, 0, 0, 0 };

struct {
    uint8 entrytype;
    uint8 entrycount;
    uint16 name[15];
} filenameEntry = { 0xC1, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };

void fat32()
{
}