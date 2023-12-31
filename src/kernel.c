#include "console.h"
#include "kernel.h"
#include "libc.h"
#include "string.h"
#include "io_ports.h"
#include "memory.h"
#include "cpu.h"
#include "fs.h"
#include "time.h"
#include "sleep.h"
#include "config.h"
#include "keyboard.h"
#include "usb.h"
#include "sh.h"
#include "helpdocs.h"
#include "sysdiag.h"
#include "devconfig.h"
#include "exec.h"
#include "panic.h"
#include "PreBoot.h"
#include "ide.h"
#include "bitmap.h"
#include "launchp.h"
#include "interface.h"
#include "process.h"

#define PORT 0x3f8          // COM1
 
#define VGA_CRT_CTRL_REG 0x3D4
#define VGA_CRT_DATA_REG 0x3D5

#define VGA_CRT_CTRL_REG 0x3D4
#define VGA_CRT_DATA_REG 0x3D5

static int init_serial() {
   outportb(PORT + 1, 0x00);    // Disable all interrupts
   outportb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   outportb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
   outportb(PORT + 1, 0x00);    //                  (hi byte)
   outportb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
   outportb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   outportb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
   outportb(PORT + 4, 0x1E);    // Set in loopback mode, test the serial chip
   outportb(PORT + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)
 
   // Check if serial is faulty (i.e: not same byte as sent)
   if(inportb(PORT + 0) != 0xAE) {
      return 1;
   }
 
   // If serial is not faulty set it in normal operation mode
   // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
   outportb(PORT + 4, 0x0F);
   return 0;
}

int is_transmit_empty() {
   return inportb(PORT + 5) & 0x20;
}
 
void write_serial(const char* str) {
   while (*str != '\0') {
      while (is_transmit_empty() == 0);

      if (*str == '\n') {
         // Move to the beginning of the next line while staying at the same X position
         outportb(PORT, '\r');  // Carriage return
         while (is_transmit_empty() == 0);
      }

      outportb(PORT, *str);
      str++;
   }
}

void pserial(const char* str) { // Use const char* for the string parameter
    write_serial(str);
    write_serial("\n");
}

void setVgaResolution(uint16 width, uint16 height) {
    // Disable sequencer to unlock CRTC registers
    outportb(VGA_CRT_CTRL_REG, 0x00);
    outportb(VGA_CRT_DATA_REG, 0x01);

    // Set width
    outportb(VGA_CRT_CTRL_REG, 0x01);  // CRT controller register to set horizontal resolution
    outportb(VGA_CRT_DATA_REG, (uint8)(width / 8 - 1));
    outportb(VGA_CRT_CTRL_REG, 0x04);  // Increment CRT controller address for sequential write
    outportb(VGA_CRT_DATA_REG, (uint8)((width / 8 - 1) >> 8));

    // Set height
    outportb(VGA_CRT_CTRL_REG, 0x12);  // CRT controller register to set vertical resolution
    outportb(VGA_CRT_DATA_REG, (uint8)(height - 1));
    outportb(VGA_CRT_CTRL_REG, 0x07);  // Increment CRT controller address for sequential write
    outportb(VGA_CRT_DATA_REG, (uint8)((height - 1) >> 8));

    // Enable sequencer
    outportb(VGA_CRT_CTRL_REG, 0x00);
    outportb(VGA_CRT_DATA_REG, 0x03);
}

void boot() {
    printf("CatKernel boot() started");
    // Initialize the file table
    for (size i = 0; i < MAX_FILES; ++i) {
        rootfs.file_table[i].filename[0] = '\0';  // Empty filename indicates an unused entry
    }
    kernmessage("Starting sysdiag");
    sysdiaginit();

    kernmessage("init_serial() on COM1");
    is_transmit_empty();
    init_serial();
    printf("                              Welcome to %CCat%CKernel\n", 0xB, 0x0, 0xE, 0x0);
    printf("Boot arguments: %s\n", args);
    //sleep(3);

    printf("Using standard VGA '%ux%u'\n", VGA_WIDTH, VGA_HEIGHT);
    kernmessage("cpuid_info(1): Attempting to get CPU info");
    cpuid_info(1);
    kernmessage("GetMemory(): Attempting to get Memory info");
    GetMemory();
    kernmessage("Showing kernel's config.h configuration \\/ ");
    printf("   hostname: %s\n", host_name);
    pserial(host_name);
    printf("   username: %s\n", username);
    pserial(username);
    printf("   versionnumber: %s\n", versionnumber);
    pserial(versionnumber);
    printf("   versionname: %s\n", vername);
    pserial(vername);
    printf("   arch: %s\n", arch);
    pserial(arch);
    printf("   prebootver: %s\n", prebootversion);
    pserial(prebootversion);
    printf("   bootargs(default): %s\n", bootargs);
    pserial(bootargs);

    kernmessage("Perfect! &rootfs created with max files from MAX_FILES");
    kernmessage("Creating '/' structure");
    create_folder(&rootfs, "bin", "/");
    create_folder(&rootfs, "boot", "/");
    create_folder(&rootfs, "cdrom", "/");
    create_folder(&rootfs, "dev", "/");
    create_folder(&rootfs, "etc", "/");
    create_folder(&rootfs, "home", "/");
    create_folder(&rootfs, "lib", "/");
    create_folder(&rootfs, "media", "/");
    create_folder(&rootfs, "mnt", "/");
    create_folder(&rootfs, "mount", "/");
    create_folder(&rootfs, "proc", "/");
    create_folder(&rootfs, "run", "/");
    create_folder(&rootfs, "sbin", "/");
    create_folder(&rootfs, "sys", "/");
    create_folder(&rootfs, "tmp", "/");
    create_folder(&rootfs, "usr", "/");
    create_folder(&rootfs, "var", "/");

    create_folder(&rootfs, "ksh", "/etc");
    create_folder(&rootfs, "bin", "/usr");
    create_folder(&rootfs, "local", "/usr");
    create_folder(&rootfs, "share", "/usr");

    list_files(&rootfs, 0);
    printf("\n");

    //Add the kernel to the list of running processes
    addProcess("kernel");

    current_directory = "/etc";

    write_to_file(&rootfs, "launchp", "");
    add_data_to_file(&rootfs, "launchp", "sh\n");

    kernmessage("Dumping kernel values to /proc");
    pserial("Dumping kernel values to /proc");
    current_directory = "/proc";
    write_to_file(&rootfs, "pre-art", art);
    write_to_file(&rootfs, "buff", buffer);
    write_to_file(&rootfs, "buff-second", buffer2);

    kernmessage("Dumping apps to /bin");
    pserial("Dumping apps to /bin");
    current_directory = "/bin";

    write_to_file(&rootfs, "print", "type:App\nprint");
    write_to_file(&rootfs, "clear", "type:App\nclear");
    write_to_file(&rootfs, "delay", "type:App\ndelay");

    current_directory = "/sbin";
    write_to_file(&rootfs, "sh", "type:App\nsh");
    kernmessage("Created /sbin/sh");
    write_to_file(&rootfs, "shutdown", "type:App\nshutdown");
    kernmessage("Created /sbin/shutdown");
    write_to_file(&rootfs, "reboot", "type:App\nreboot");
    kernmessage("Created /sbin/reboot");

    kernmessage("Freeing system memory");

    size size = 6 * 1024 * 1024; // 6 MB in bytes
    //void* memory = k_malloc(size);
    k_malloc(size);

    kernmessage("Allocated 6000 KB using k_malloc()");

    kernmessage("Setting hostname to defaults from 'defaulthostname'");
    current_directory = "/etc";
    write_to_file(&rootfs, "hostname", defaulthostname);
    read_from_file(&rootfs, "hostname", buffer, sizeof(buffer), 1);

    strcpy(host_name, buffer);
    kernmessage("Copying kernel values to proc");
    current_directory = "/proc";
    write_to_file(&rootfs, "arch", arch);
    printf("%C   Created /proc/arch\n", 0x8, 0x0);

    write_to_file(&rootfs, "args", bootargs);
    printf("%C   Created /proc/args\n", 0x8, 0x0);

    write_to_file(&rootfs, "version", vername);
    add_data_to_file(&rootfs, "version", versionnumber);
    printf("%C   Created /proc/version\n", 0x8, 0x0);

    write_to_file(&rootfs, "versionnum", versionnumber);
    printf("%C   Created /proc/versionnum\n", 0x8, 0x0);

    write_to_file(&rootfs, "prebootver", prebootversion);
    printf("%C   Created /proc/prebootver\n", 0x8, 0x0);

    write_to_file(&rootfs, "ostype", "Catkernel");
    printf("%C   Created /proc/ostype\n", 0x8, 0x0);

    write_to_file(&rootfs, "cpu", brand);
    printf("%C   Created /proc/cpu\n", 0x8, 0x0);

    write_to_file(&rootfs, "vga", "80x25");
    printf("%C   Created /proc/vga\n", 0x8, 0x0);

    printf("Autoconfiguring devices...\n");
    BootDevConfig();

    kernmessage("Detecting ATA drives");
    //printf("\nExample\n");
    //const uint32 LBA = 0;
    //const uint8 NO_OF_SECTORS = 1;
    //char buf[ATA_SECTOR_SIZE] = {0};
    ata_init();

    current_directory = "/boot";

    // root user
    kernmessage("Setting up default user");
    current_directory = "/etc";
    strcpy(rootUser.username, "root");
    strcpy(rootUser.shell, "/bin/sh");
    strcpy(username, rootUser.username);
    write_to_file(&rootfs, "session", rootUser.username);
    write_to_file(&rootfs, "motd", "Welcome to Catkernel. You are using k_sh, Catkernel's built-in shell.\nYou can change this message in /etc/motd");
    current_directory = "/home";
    create_folder(&rootfs, "root", "/home");
    current_directory = "/home/root";
    write_to_file(&rootfs, "readme", "Welcome to CatK! This is the default root user.\n\nThank you for using CatK. Built with love and care by Rodmatronics, Irix, and a few other awesome dudes :3");
    write_to_file(&rootfs, "history", "");

    bootlogo = 0;

    kernmessage("Creating needed files in the rootFS");
    current_directory = "/bin";
    write_to_file(&rootfs, "game", "type:App\nclear\ncatascii-happy\nprint -----------------------------------------------\nprint Well hello, this is a simple game.\nprint -----------------------------------------------\nprint Press [ENTER]\nread\nclear\nprint COMMENCING SLEEP..\ncatascii-lookup\nprint -----------------------------------------------\nprint ?\nprint -----------------------------------------------\nprint Press [ENTER]\nread\ndelay\nclear\\ncatascii-tired\nprint_dark -----------------------------------------------\nprint_dark ...\nprint_dark -----------------------------------------------\ndelay\nclear\ncatascii-sleep\ndelay");
    write_to_file(&rootfs, "compat-readme", "This folder is not used by CatK in any resonable way.\n\nThis is just here for UNIX compatibility :3");
    createhelpdocs();
    create_folder(&rootfs, "desktop", "/bin");
    current_directory = "/bin/desktop";
    write_to_file(&rootfs, "vga", "type:App\ngraphics_init");
    kernmessage("Created /sbin/vga");

    kernmessage("Setting up ENV variables");
    current_directory = "/var";
    write_to_file(&rootfs, "CATKVER", versionnumber);
    write_to_file(&rootfs, "HOSTNAME", "catk");
    write_to_file(&rootfs, "LANG", "en");
    write_to_file(&rootfs, "USER", "root");
    write_to_file(&rootfs, "HOME", "/home");
    write_to_file(&rootfs, "SHELL", "k_sh");

    pserial("Boot should be finished. Starting a shell");

    kernmessage("Starting launchp...");

    current_directory = "/";
    console_init(COLOR_WHITE, COLOR_BLACK);
    launchp();
}

void kmain() {
    // small delay to let grub finish it's thing
    int i;for (int i = 0; i < 10000; ++i) {for (int j = 0; j < 1800; ++j) {}}
    
    console_init(COLOR_GREY, COLOR_BLACK);
    PreBoot();
}

void kernmessage(const char* str) { // Use const char* for the string parameter
    printf("kernel: %s\n", str); // Print the message and the string   
    write_serial(str);
    write_serial("\n");
    char* workingdirectory = current_directory;
    current_directory = "/etc";
    add_data_to_file(&rootfs, "logs", "\n");
    add_data_to_file(&rootfs, "logs", str);
    current_directory = workingdirectory;
}

/*
void catkmessagefixed(int NUM) { // Use const char* for the string parameter
   
    //do nothing
}*/