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
#include "sysdiag.h"
#include "smbios.h"

void boot() {
    bootmessage("CatKernel boot() started");
    printf("                              Welcome to %CCat%CKernel\n", 0xB, 0x0, 0xE, 0x0);
    printf("Boot arguments: %s\n", args);
    //sleep(3);

    printf("Using standard VGA '%ux%u'\n", VGA_WIDTH, VGA_HEIGHT);
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

    bootmessage("Getting SMBIOS\\/ ");
    initsmbiosfind();

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

    bootmessage("Starting sysdiag");
    sysdiaginit();

    bootmessage("Freeing system memory");

    size size = 6 * 1024 * 1024; // 6 MB in bytes
    //void* memory = k_malloc(size);
    k_malloc(size);

    bootmessage("Allocated 6000 KB using k_malloc");

    bootmessage("Keyboard init ");
    init_keyboard();

    bootmessage("Setting hostname to defaults from 'defaulthostname'");
    current_directory = "/etc";
    write_to_file(&rootfs, "hostname", defaulthostname);
    read_from_file(&rootfs, "hostname", buffer, sizeof(buffer), 1);

    strcpy(host_name, buffer);

    bootmessage("Copying kernel values to proc");
    current_directory = "/proc";
    write_to_file(&rootfs, "arch", arch);
    printf("%C   Created /proc/arch\n", 0x8, 0x0);

    write_to_file(&rootfs, "args", bootargs);
    printf("%C   Created /proc/args\n", 0x8, 0x0);

    write_to_file(&rootfs, "version", vername);
    add_data_to_file(&rootfs, "version", versionnumber);
    printf("%C   Created /proc/version\n", 0x8, 0x0);

    write_to_file(&rootfs, "prebootver", prebootversion);
    printf("%C   Created /proc/prebootver\n", 0x8, 0x0);

    write_to_file(&rootfs, "ostype", "Catkernel");
    printf("%C   Created /proc/ostype\n", 0x8, 0x0);

    write_to_file(&rootfs, "cpu", brand);
    printf("%C   Created /proc/cpu\n", 0x8, 0x0);

    write_to_file(&rootfs, "vga", "80x25");
    printf("%C   Created /proc/vga\n", 0x8, 0x0);

    bootmessage("Copying kernel values to dev");
    current_directory = "/dev";

    write_to_file(&rootfs, "console", "1");
    printf("%C   Created /dev/console\n", 0x8, 0x0);

    write_to_file(&rootfs, "null", "0");
    printf("%C   Created /dev/console\n", 0x8, 0x0);

    write_to_file(&rootfs, "random", "1234");
    printf("%C   Created /dev/console\n", 0x8, 0x0);

    write_to_file(&rootfs, "tty0", "1");
    printf("%C   Created /dev/tty1\n", 0x8, 0x0);

    write_to_file(&rootfs, "tty1", "0");
    printf("%C   Created /dev/tty1\n", 0x8, 0x0);

    write_to_file(&rootfs, "tty2", "0");
    printf("%C   Created /proc/tty2\n", 0x8, 0x0);

    write_to_file(&rootfs, "tty3", "0");
    printf("%C   Created /proc/tty3\n", 0x8, 0x0);

    write_to_file(&rootfs, "tty4", "0");
    printf("%C   Created /proc/tty4\n", 0x8, 0x0);

    write_to_file(&rootfs, "tty5", "0");
    printf("%C   Created /proc/tty5\n", 0x8, 0x0);

    write_to_file(&rootfs, "tty6", "0");
    printf("%C   Created /proc/tty6\n", 0x8, 0x0);

    write_to_file(&rootfs, "tty7", "0");
    printf("%C   Created /dev/tty1\n", 0x8, 0x0);

    bootmessage("Setting up default user");
    // root user
    strcpy(rootUser.username, "root");
    strcpy(rootUser.shell, "/bin/sh");
    strcpy(username, rootUser.username);
    write_to_file(&rootfs, "session.catk", rootUser.username);
    create_folder(&rootfs, "/home", "/");
    create_folder(&rootfs, "/root", "/home");

    bootmessage("Finishing up...");
    //char* shell = "k_sh";

current_directory = "/bin";
    write_to_file(&rootfs, "game", "type:App\nclear\ncatascii-happy\nprint -----------------------------------------------\nprint Well hello, this is a simple game.\nprint -----------------------------------------------\nprint Press [ENTER]\nread\nclear\nprint COMMENCING SLEEP..\ncatascii-lookup\nprint -----------------------------------------------\nprint ?\nprint -----------------------------------------------\nprint Press [ENTER]\nread\ndelay\nclear\\ncatascii-tired\nprint_dark -----------------------------------------------\nprint_dark ...\nprint_dark -----------------------------------------------\ndelay\nclear\ncatascii-sleep\ndelay");

    current_directory = "/";

    console_init(COLOR_WHITE, COLOR_BLACK);
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
