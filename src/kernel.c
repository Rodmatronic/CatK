#include "console.h"
#include "kernel.h"
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

void boot() {
    bootmessage("CatKernel (unfucked) boot() started");
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
    execute_file(&rootfs, "init");
    panic("Cannot mount &rootfs!");

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
