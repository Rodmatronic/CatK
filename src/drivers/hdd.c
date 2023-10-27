#include "io_ports.h"

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

// Function to check for the presence of an ATA drive
int isATADrivePresent(int driveNumber) {
    // Select the ATA drive (e.g., ada0, ada1, ...)
    outportb(ATA_REG_DRIVE, ATA_DEVICE_SELECT | driveNumber);

    // Check the status
    unsigned char status = inportb(ATA_REG_STATUS);

    // Bit 4 (BSY) should be clear, and bit 7 (DRDY) should be set
    if (!(status & 0x80) && (status & 0x40)) {
        return 1; // ATA drive is present
    } else {
        return 0; // No ATA drive detected
    }
}

void finddrives() {
    int maxDriveNumber = 5; // Check for drives ada0 to ada5

    for (int driveNumber = 0; driveNumber <= maxDriveNumber; driveNumber++) {
        if (isATADrivePresent(driveNumber)) {
            printf("hdd%d: Drive ada%d is attached.\n", driveNumber);
        } else {
            printf("hdd%d: No drive ada%d detected.\n", driveNumber);
        }
    }

}
