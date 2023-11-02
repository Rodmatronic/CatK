

#include "string.h"
#include "io_ports.h"
// I/O Ports for RTC
#define RTC_ADDRESS_PORT 0x70
#define RTC_DATA_PORT 0x71

// RTC Register Addresses
#define RTC_SECONDS 0x00
#define RTC_MINUTES 0x02
#define RTC_HOURS 0x04
#define RTC_DAY_OF_MONTH 0x07
#define RTC_MONTH 0x08
#define RTC_YEAR 0x09

uint8 seconds, minutes, hours, day, month, year;

uint8 seed;

// Function to write to RTC register
void write_RTC_register(uint8 reg, uint8 value) {
    outportb(RTC_ADDRESS_PORT, reg);
    outportb(RTC_DATA_PORT, value);
}

// Function to read from RTC register
uint8 read_RTC_register(uint8 reg) {
    outportb(RTC_ADDRESS_PORT, reg);
    return inportb(RTC_DATA_PORT);
}

// Define an array to store the power-on time
uint8 powerontime[3] = {0}; // Initialize to 0, representing hours, minutes, seconds

void powerontimes() {
    if (powerontime[0] == 0 && powerontime[1] == 0 && powerontime[2] == 0) {
        // Only store the time if it hasn't been set previously
        powerontime[0] = hours;
        powerontime[1] = minutes;
        powerontime[2] = seconds;

        seed = seconds;
    }
}

void printPowerOnTime() {
    printf("Power-on time: %02x:%02x:%02x\n", powerontime[0], powerontime[1], powerontime[2]);
}

void GetCurrentTime() {

    // Ensure the RTC is not updated during read
    write_RTC_register(0x0B, read_RTC_register(0x0B) | 0x80);

    // Read RTC registers
    seconds = read_RTC_register(RTC_SECONDS);
    minutes = read_RTC_register(RTC_MINUTES);
    hours = read_RTC_register(RTC_HOURS);
    day = read_RTC_register(RTC_DAY_OF_MONTH);
    month = read_RTC_register(RTC_MONTH);
    year = read_RTC_register(RTC_YEAR);

    // Clear the update-in-progress flag
    write_RTC_register(0x0B, read_RTC_register(0x0B) & 0x7F);

    printf("%02x:%02x:%02x %02x/%02x/%02x\n", hours, minutes, seconds, month, day, year);

    // Call the function to store power-on time
    powerontimes();
}