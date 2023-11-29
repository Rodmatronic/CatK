#include "console.h"
#include "kernel.h"
#include "libc.h"
#include "string.h"
#include "io_ports.h"
#include "PreBoot.c"
#include "memory.h"
#include "cpu.h"
#include "fs.h"
#include "time.h"
#include "sleep.h"
#include "config.h"
#include "keyboard.h"
#include "usb.h"
#include "sh.h"

void boot() {
    bootmessage("CatKernel boot() started");
    printf("Boot arguments: %s\n", args);
    bootmessage("cpuid_info(1): Attempting to get CPU info");
    cpuid_info(1);
    bootmessage("GetMemory(): Attempting to get Memory info");
    GetMemory();
    bootmessage("Showing kernel's config.h configuration \\/ ");
    printf("   hostname: %s\n", host_name);
    printf("   username: %s\n", username);
    printf("   versionnumber: %s\n", versionnumber);
    printf("   versionname: %s\n", vername);
    printf("   arch: %s\n", arch);
    printf("   prebootver: %s\n", prebootversion);
    printf("   bootargs(default): %s\n", bootargs);

    // Initialize the file table
    for (size i = 0; i < MAX_FILES; ++i) {
        rootfs.file_table[i].filename[0] = '\0';  // Empty filename indicates an unused entry
    }

    bootmessage("Perfect! &rootfs created with max files from MAX_FILES");
    bootmessage("Creating '/' structure");
    create_folder(&rootfs, "/bin", "/");
    create_folder(&rootfs, "/boot", "/");
    create_folder(&rootfs, "/cdrom", "/");
    create_folder(&rootfs, "/dev", "/");
    create_folder(&rootfs, "/etc", "/");
    create_folder(&rootfs, "/home", "/");
    create_folder(&rootfs, "/lib", "/");
    create_folder(&rootfs, "/media", "/");
    create_folder(&rootfs, "/mnt", "/");
    create_folder(&rootfs, "/proc", "/");
    create_folder(&rootfs, "/run", "/");
    create_folder(&rootfs, "/sbin", "/");
    create_folder(&rootfs, "/sys", "/");
    create_folder(&rootfs, "/tmp", "/");
    create_folder(&rootfs, "/usr", "/");
    create_folder(&rootfs, "/var", "/");
    list_files(&rootfs, 0);
    printf("\n");

    bootmessage("Freeing system memory");

    size size = 6 * 1024 * 1024; // 6 MB in bytes
    //void* memory = k_malloc(size);
    k_malloc(size);

    bootmessage("Allocated 6000 KB using k_malloc");

    bootmessage("Keyboard init ");
    init_keyboard();

    console_init(COLOR_WHITE, COLOR_BLACK);
    cursor_pos_y = 25;
    k_sh();

}

void kmain() {
    console_init(COLOR_GREY, COLOR_BLACK);
    PreBoot();
}

void bootmessage(const char* str) { // Use const char* for the string parameter
    printf("kernel: %s\n", str); // Print the message and the string
}

/*
void catkmessagefixed(int NUM) { // Use const char* for the string parameter
   
    //do nothing
}*/
