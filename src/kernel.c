#include "kernel.h"
#include "console.h"
#include "string.h"
#include "io_ports.h"
#include "drivers/time.c"
#include "drivers/memory.c"
#include "drivers/cpu.c"
#include "drivers/panic.c"
#include "drivers/cdrompresent.c"
#include "drivers/hdd.c"
#include "drivers/acpi.c"
#include "drivers/keyboard.c"
#include "drivers/ethernet.c"
#include "bin/sh.c"
#include "bin/read.c"
#include "drivers/fs.c"
#include "config.catk"
#include "fs.h"
#include "PreBoot.c"

void printversion()
{
    printf("\n%s\n", vername);
}

void boot() {

    if (bootargs == "quiet")
    {
        console_init(COLOR_BLACK, COLOR_BLACK);
    }

    printf("<--BOOT-->\n");
    printf("Created by Rodmatronics. Thank you for using CatK!\n");
    printf("This is a BSD inspired kernel.\n");
    printf("    It does NOT use any BSD source code\n");
    bootmessage("Getting system clock...");
    printf("time:");
    GetCurrentTime();
    bootmessage("Getting memory...");
    GetMemory();
    bootmessage("Using config.catk for kernel configuration & args");
    printf("Kernel version: %d\n", versionnumber);
    printf("PreBoot version: %d\n", prebootversion);
    printf("Kernel args: %s\n\n", bootargs); // Print the string
    bootmessage("Getting CPU info...");
    cpuinfo();
    bootmessage("Scanning for devices...");
    isCDROMDrivePresent();
    finddrives();
    printf("kernel: Trying to mount root...\n");
    initfilesystem(&root);
    ls(&root);

    // Print the structure
    printf("Root: %s\n", root.name);
    for (int i = 0; i < root.subfolderCount; i++) {
        printf("  - %s\n", root.subfolders[i]->name);
    }

    // Assuming NAT configuration
    const char* statusNAT = isEthernetPluggedIn();
    printf("eth0: NAT Network Status: %s\n", statusNAT);

    const char* statusBridged = isEthernetPluggedIn();
    printf("eth0: Bridged Network Status: %s\n", statusBridged);

    init_keyboard();
    printf_dark("Enter full pathname for shell or RETURN for /bin/sh: \n");
    read();
    console_init(COLOR_WHITE, COLOR_BLACK);
    sh();
}

void kmain() {
    console_init(COLOR_GREY, COLOR_BLACK);

    PreBoot();
}

// Function to wait for a specified number of seconds
void wait_seconds(int seconds) {
    uint8 start_time = read_RTC_register(RTC_SECONDS);

    while (1) {
        uint8 current_time = read_RTC_register(RTC_SECONDS);
        if (current_time != start_time) {
            if (current_time < start_time) {
                current_time += 1; // Handle rollover from 59 to 00
            }

            if (current_time - start_time >= seconds) {
                break;
            }
        }
    }
}

void bootmessage(const char* str) { // Use const char* for the string parameter
    printf("kernel: %s\n", str); // Print the message and the string
}