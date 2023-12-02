#include "string.h"
#include "io_ports.h"
#include "time.h"
#include "console.h"
#include "libc.h"
#include "time.h"

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
    // Assuming powerontime is an array of integers with size 3
    if (powerontime[0] == 0 && powerontime[1] == 0 && powerontime[2] == 0) {
        // Only store the time if it hasn't been set previously
        powerontime[0] = hours;
        powerontime[1] = minutes;
        powerontime[2] = seconds;

        seed = seconds;

        // Convert integers to ASCII characters
        char poweronTimeString[10]; // Adjust the size according to your needs
        int index = 0;

        // Append hours
        poweronTimeString[index++] = ((hours / 10) % 10) + '0';
        poweronTimeString[index++] = (hours % 10) + '0';
        poweronTimeString[index++] = ':';

        // Append minutes
        poweronTimeString[index++] = ((minutes / 10) % 10) + '0';
        poweronTimeString[index++] = (minutes % 10) + '0';
        poweronTimeString[index++] = ':';

        // Append seconds
        poweronTimeString[index++] = ((seconds / 10) % 10) + '0';
        poweronTimeString[index++] = (seconds % 10) + '0';

        // Null-terminate the string
        poweronTimeString[index] = '\0';

        // Write the power-on time string to the file
        //char* workingdir = current_directory;
        //current_directory = "/proc";
        //write_to_file(&rootfs, "powerontime", poweronTimeString);
        //current_directory = workingdir;
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

    // Convert integers to ASCII characters
    char timeString[20]; // Adjust the size according to your needs
    int index = 0;

    // Append hours
    timeString[index++] = ((hours / 10) % 10) + '0';
    timeString[index++] = (hours % 10) + '0';
    timeString[index++] = ':';

    // Append minutes
    timeString[index++] = ((minutes / 10) % 10) + '0';
    timeString[index++] = (minutes % 10) + '0';
    timeString[index++] = ':';

    // Append seconds
    timeString[index++] = ((seconds / 10) % 10) + '0';
    timeString[index++] = (seconds % 10) + '0';

    // Append date
    timeString[index++] = ' ';
    timeString[index++] = ((month / 10) % 10) + '0';
    timeString[index++] = (month % 10) + '0';
    timeString[index++] = '/';
    timeString[index++] = ((day / 10) % 10) + '0';
    timeString[index++] = (day % 10) + '0';
    timeString[index++] = '/';
    timeString[index++] = ((year / 10) % 10) + '0';
    timeString[index++] = (year % 10) + '0';

    // Null-terminate the string
    timeString[index] = '\0';

    // Write the time string to the file
    //write_to_file(&rootfs, "time", timeString);

    // Call the function to store power-on time
    powerontimes();
}