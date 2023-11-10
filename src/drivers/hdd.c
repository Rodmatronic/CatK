#include "types.h"
#include "io_ports.h"


#define ATA_IDENTIFY_PORT   0x1F7   // Command port for ATA identify
#define ATA_STATUS_PORT     0x1F7   // Status port for ATA
#define ATA_DRIVE_SELECT    0xE0    // Drive select bits for master drive
#define ATA_DRIVE_BIT       4       // Bit for selecting the slave drive

int listdrivebits() {
    for (int drive = 0; drive < 8; ++drive) {
        outportb(ATA_DRIVE_SELECT | (drive << ATA_DRIVE_BIT), ATA_IDENTIFY_PORT);

        // Send NOP command and check the status
        outportb(0x00, ATA_STATUS_PORT);

        // Read the status
        unsigned char status = inportb(ATA_STATUS_PORT);

        // Check if the drive is active
        if (status != 0x00 && status != 0xFF) {
            // Drive is active, do something here
            printf("Drive ATA %d is active\n", drive + 1);
        }
    }
    
    // If no active drives were found
    printf("DUMMY\n");
}