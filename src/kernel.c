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
#include "drivers/syspw.c"
#include "drivers/keyboard.c"
#include "drivers/ethernet.c"
#include "bin/sh.c"
#include "bin/top.c"
#include "bin/sleep.c"
#include "bin/read.c"
#include "drivers/fs.c"
#include "config.h"
#include "fs.h"
#include "PreBoot.c"
#include "drivers/crypto.c"
#include "sbin/init.c"
#include "sbin/termcolors.c"

void printversion()
{
    printf("%s", vername);
}

// Function to create a new process
int fork(void (*entry)()) {
    if (next_pid >= MAX_PROCESSES) {
        // Max number of processes reached
        return -1;
    }

    int pid = next_pid++;
    processes[pid - 1].id = pid;
    processes[pid - 1].entry = entry;

    return pid;
}

void execute_process(int pid) {
    if (pid > 0 && pid <= next_pid) {
        // Call the entry function for the process
        processes[pid - 1].entry();
        // Mark the process as terminated
        processes[pid - 1].state = 1;
        // Return to kernel main loop or scheduler
    }
}

void init_processes() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].id = -1;
        processes[i].state = 0;
    }
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
    printf("Kernel version: %s\n", versionnumber);
    printf("PreBoot version: %s\n", prebootversion);
    printf("Kernel args: %s\n\n", bootargs); // Print the string

    init_processes();

    init();
}

void kmain() {
    console_init(COLOR_GREY, COLOR_BLACK);
    PreBoot();
}

void bootmessage(const char* str) { // Use const char* for the string parameter
    printf("kernel: %s\n", str); // Print the message and the string
}

void catkmessage(int NUM, ...) { // Use const char* for the string parameter
    if (NUM == 1)
    {
        printf("       [ "); // Print the message and the string
        printf_green("OK"); // Print the message and the string
        printf(" ]\n"); // Print the message and the string
    }else
    if (NUM == 2)
    {
        printf("       [ "); // Print the message and the string
        printf_yellow("WARN");
        printf(" ]\n");
    }else
    if (NUM == 3)
    {
        printf("       [ "); // Print the message and the string
        printf_red("ERR!");
        printf(" ]\n");
    }
}