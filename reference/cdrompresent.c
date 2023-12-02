#include "io_ports.h"
#include "libc.h"
#include "console.h"

// I/O Ports for ATA/ATAPI
#define ATA_REG_DATA 0x1F0 // Data Register
#define ATA_REG_FEATURES 0x1F1 // Features Register
#define ATA_REG_ERROR 0x1F1 // Error Register (write)
#define ATA_REG_SECCOUNT0 0x1F2 // Sector Count Register (LBA)
#define ATA_REG_LBA0 0x1F3 // LBA Low Register (LBA)
#define ATA_REG_LBA1 0x1F4 // LBA Mid Register (LBA)
#define ATA_REG_LBA2 0x1F5 // LBA High Register (LBA)
#define ATA_REG_DRIVE 0x1F6 // Drive/Head Register
#define ATA_REG_COMMAND 0x1F7 // Command Register
#define ATA_REG_STATUS 0x1F7 // Status Register (read)

// ATA/ATAPI commands
#define ATA_IDENTIFY 0xEC // Identify Drive
#define ATA_DEVICE_SELECT 0xA0 // Select Device

// Function to check for the presence of a CD-ROM drive
void isCDROMDrivePresent() {
    // Select the CD-ROM drive
    outportb(ATA_REG_DRIVE, ATA_DEVICE_SELECT);

    // Check the status
    unsigned char status = inportb(ATA_REG_STATUS);

    // Bit 4 (BSY) should be clear, and bit 7 (DRDY) should be set
    if (!(status & 0x80) && (status & 0x40)) {
        printf("cd0: cd drive detected\n");
    } else {
        printf("cd0: No cd drive detected\n");
    }
}
