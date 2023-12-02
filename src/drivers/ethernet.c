#include "types.h"
#include "io_ports.h"
#include "libc.h"
#include "console.h"

// Define the I/O ports
#define ETH_STATUS_PORT 0x2
#define ETH_STATUS_REGISTER 0x00

// Function to read a byte from an I/O port
uint8 ethplugged(uint16 port) {
    uint8 data;
    asm volatile("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

// Function to check Ethernet port status
const char* isEthernetPluggedIn() {
    
    // Read the status register of the NIC
    uint8 status = ethplugged(ETH_STATUS_PORT + ETH_STATUS_REGISTER);

    // Check the link status bit (bit 2)
    if (status & (1 << 2)) {
        printf("\nDetected Ethernet!");
        return "1";
    } else {
        printf("\nDid not detect Ethernet");
        return "0";
    }
}