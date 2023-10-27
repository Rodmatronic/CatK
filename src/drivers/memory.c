/*
 * Get the system memory
 * Written by Rodmatronics
*/

#include "string.h"
#include "io_ports.h"
// I/O Ports for RTC
#define RTC_ADDRESS_PORT 0x70
#define RTC_DATA_PORT 0x71

void GetMemory() {
    // Read the extended memory size from the BIOS Data Area (BDA)
    unsigned short* bda_memory_size = (unsigned short*)0x413;
    unsigned int conventional_memory_kb = *bda_memory_size;

    // Convert conventional memory size to bytes
    unsigned int conventional_memory_bytes = conventional_memory_kb * 1024;

    // Read extended memory size from another memory address (e.g., 0x15 or 0x88, depending on your system)
    unsigned short* extended_memory_size = (unsigned short*)0x88; // You may need to adjust this address
    unsigned int extended_memory_kb = *extended_memory_size;

    // Convert extended memory size to bytes
    unsigned int extended_memory_bytes = extended_memory_kb * 1024;

    printf("Memory: Conventional memory: %u bytes (%u KB)\n", conventional_memory_bytes, conventional_memory_kb);
    printf("Memory: Extended memory: %u bytes (%u KB)\n", extended_memory_bytes, extended_memory_kb);
}