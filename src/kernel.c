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
#include "config.catk"
#include "fs.h"
#include "PreBoot.c"
#include "drivers/crypto.c"

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

void shell_process() {
    // Your shell process code here
    console_init(COLOR_WHITE, COLOR_BLACK);
    next_pid = 1;
    sh();
    // Once the shell exits, you may want to terminate the process.
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

    init_processes();

    // Create some sample processes
    int process1 = fork(process1);
    int process2 = fork(process2);
    int process3 = fork(process3);

    // Your existing user
    struct User rootuser;
    strcpy(rootuser.username, "root");
    strcpy(rootuser.shell, "/bin/sh");

    // New user entry
    struct User newUser;
    strcpy(newUser.username, "CatK");
    strcpy(newUser.shell, "/bin/bash");

    // Check if the user already exists
    if (strcmp(rootuser.username, newUser.username) == 0) {
        printf("User already exists. Please choose a different username.\n");
    } else {
        // If the user doesn't exist, you can replace the existing user with the new one.
        rootuser = newUser;
        printf("New user has been added.\n");
    }

    init_keyboard();
    if (bootargs != "quiet")
    {
        printf_dark("Enter full pathname for shell or RETURN for /bin/sh: \n");
        read();
    }

    console_init(COLOR_WHITE, COLOR_BLACK);
    // Create a shell process
    int shell_pid = fork(shell_process);

    if (shell_pid > 0) {
        // Execute the shell process
        execute_process(shell_pid);
    }
}

void kmain() {
    console_init(COLOR_GREY, COLOR_BLACK);
    PreBoot();
}

void bootmessage(const char* str) { // Use const char* for the string parameter
    printf("kernel: %s\n", str); // Print the message and the string
}