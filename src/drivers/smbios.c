#include "fs.h"
#include "libc.h"

// Function to print SMBIOS information
void printSMBIOSInfo(unsigned char *mem) {
    printf("SMBIOS Signature Found!\n");
    printf("SMBIOS Version: %u%u%u%u\n", mem[6], mem[7], mem[8], mem[9]);
}

// Function to write SMBIOS information
void writeSMBIOSInfo(unsigned char *mem) {
    char smbiosVersion[5]; // Assuming you want a 4-character version plus the null terminator

    // Using sprintf to concatenate the values into a string
    snprintf(smbiosVersion, "%u%u%u%u", mem[6], mem[7], mem[8], mem[9]);
    current_directory = "/proc";
    write_to_file(&rootfs, "smbios", smbiosVersion);
    printf("%C   Created /proc/smbios\n", 0x8, 0x0);
}

void initsmbiosfind()
{
    char *mem = (unsigned char *) 0xF0000;
    int length, i;
    unsigned char checksum;
    while ((unsigned int)mem < 0x100000) {
        if (mem[0] == '_' && mem[1] == 'S' && mem[2] == 'M' && mem[3] == '_') {
            length = mem[5];
            checksum = 0;

            for (i = 0; i < length; i++) {
                checksum += mem[i];
            }

            if (checksum == 0) {
                printSMBIOSInfo(mem);
                writeSMBIOSInfo(mem);
                break;
            }
        }
        mem += 16;
    }

    if ((unsigned int) mem == 0x100000) {
        bootmessage("No SMBIOS found! Continuing...");
    }
}