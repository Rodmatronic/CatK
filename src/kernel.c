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

void printversion()
{
    printf("\n%s\n", vername);
}

void boot() {
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
    printf_dark("kernel: Trying to mount root...\n");
    initfilesystem(&root);
    // List the contents of the current directory
    ls(&root);
    // Let's list the contents of the root folder using the 'ls' command.

    // Print the structure
    printf("Root: %s\n", root.name);
    for (int i = 0; i < root.subfolderCount; i++) {
        printf("  - %s\n", root.subfolders[i]->name);
    }

    // Assuming NAT configuration
    const char* statusNAT = isEthernetPluggedIn();
    printf("eth0: NAT Network Status: %s\n", statusNAT);

    // Now, configure for bridged network
    // Modify network settings in your virtualization software
    // Then, check the status again
    const char* statusBridged = isEthernetPluggedIn();
    printf("eth0: Bridged Network Status: %s\n", statusBridged);

    init_keyboard();
    printf_dark("Enter full pathname for shell or RETURN for /bin/sh: \n");
    read();
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

void awesomepeeps()
{
    printf("Some awesome people (in no particular order)\n");
    printf("Gamer117\n");
    printf("Shea\n");
    printf("Smart Dolphin\n");
    printf("Noodlenoggin\n");
    printf("The Funny Looney clown\n");
    printf("Stamps\n");
    printf("Mati\n");
    printf("Vincells\n");
    printf("Gyl\n");
    printf("Zeo\n");
    printf("4vul\n");
    printf("Noelle\n");
    printf("maddycabin\n");
}

void PreBoot()
{
    console_gotoxy(0, 0);
    printf(" _______   _____   _______  ___   _ \n");
    printf("|   ____| /  _  \\ |       ||   | | |\n");
    printf("|  |     |  | |  ||_     _||   |_| |\n");
    printf("|  |     |  |_|  |  |   |  |      _|\n");
    printf("|  |     |       |  |   |  |     |_ \n");
    printf("|  |____ |   _   |  |   |  |    _  |\n");
    printf("|_______||__| |__|  |___|  |___| |_|\n\n");

    printf("+==================================+\n");
    printf("|    CatK PreBoot\n");
    printf("| 1. Continue booting kernel\n");
    printf("|\n");
    printf("|\n");
    printf("|\n");
    printf("|\n");
    printf("|\n");
    printf("|\n");
    printf("|\n");
    printf("+==================================+\n");

    console_gotoxy(45, 10);
    printf_blue("       _      _");
        console_gotoxy(45, 9);
    printf_blue("      / \\    / \\");
        console_gotoxy(45, 10);
    printf_blue("     /   \\__/   \\");
        console_gotoxy(45, 11);
    printf_blue("    /            \\");
        console_gotoxy(45, 12);
    printf_blue("   |    |    |    |");
        console_gotoxy(45, 13);
    printf_blue("  =|      -       |=");
        console_gotoxy(45, 14);
    printf_blue("  =\\     \\/\\/     /=");
        console_gotoxy(45, 15);
    printf_blue("    \\            /");
        console_gotoxy(45, 16);
    printf_blue("     =====\\/=====");
        console_gotoxy(45, 17);
    printf_blue("        (CatK)");
        console_gotoxy(45, 18);

    console_gotoxy(4, 12);
    printf("Booting in 3");
    wait_seconds(1);
    console_gotoxy(4, 12);
    printf("Booting in 2");
    wait_seconds(1);
    console_gotoxy(4, 12);
    printf("Booting in 1");
    wait_seconds(1);
    console_gotoxy(0, 15);
    console_init(COLOR_WHITE, COLOR_BLACK);
    boot();
}

void bootmessage(const char* str) { // Use const char* for the string parameter
    printf("kernel: %s\n", str); // Print the message and the string
}