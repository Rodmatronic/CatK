#include "console.h"
#include "kernel.h"
#include "libc.h"
#include "string.h"
#include "io_ports.h"
#include "memory.h"
#include "cpu.h"
#include "fs.h"
#include "time.h"
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
#include "GDT.h"
#include "IDT.h"
#include "timer.h"
#include "multiboot.h"
#include "pmm.h"
#include "vesa.h"
#include "kheap.h"

#define PORT 0x3f8          // COM1
 
#define VGA_CRT_CTRL_REG 0x3D4
#define VGA_CRT_DATA_REG 0x3D5

#define VGA_CRT_CTRL_REG 0x3D4
#define VGA_CRT_DATA_REG 0x3D5

void bootart();

KERNEL_MEMORY_MAP g_kmap;

int get_kernel_memory_map(KERNEL_MEMORY_MAP *kmap, MULTIBOOT_INFO *mboot_info) {
    uint32 i;
    
    if (kmap == NULL) return -1;
    kmap->kernel.k_start_addr = (uint32)&__kernel_section_start;
    kmap->kernel.k_end_addr = (uint32)&__kernel_section_end;
    kmap->kernel.k_len = ((uint32)&__kernel_section_end - (uint32)&__kernel_section_start);

    kmap->kernel.text_start_addr = (uint32)&__kernel_text_section_start;
    kmap->kernel.text_end_addr = (uint32)&__kernel_text_section_end;
    kmap->kernel.text_len = ((uint32)&__kernel_text_section_end - (uint32)&__kernel_text_section_start);

    kmap->kernel.data_start_addr = (uint32)&__kernel_data_section_start;
    kmap->kernel.data_end_addr = (uint32)&__kernel_data_section_end; 
    kmap->kernel.data_len = ((uint32)&__kernel_data_section_end - (uint32)&__kernel_data_section_start);

    kmap->kernel.rodata_start_addr = (uint32)&__kernel_rodata_section_start;
    kmap->kernel.rodata_end_addr = (uint32)&__kernel_rodata_section_end;
    kmap->kernel.rodata_len = ((uint32)&__kernel_rodata_section_end - (uint32)&__kernel_rodata_section_start);

    kmap->kernel.bss_start_addr = (uint32)&__kernel_bss_section_start;
    kmap->kernel.bss_end_addr = (uint32)&__kernel_bss_section_end; 
    kmap->kernel.bss_len = ((uint32)&__kernel_bss_section_end - (uint32)&__kernel_bss_section_start);

    kmap->system.total_memory = mboot_info->mem_low + mboot_info->mem_high;

    for (i = 0; i < mboot_info->mmap_length; i += sizeof(MULTIBOOT_MEMORY_MAP)) {
        MULTIBOOT_MEMORY_MAP *mmap = (MULTIBOOT_MEMORY_MAP *)(mboot_info->mmap_addr + i);
        if (mmap->type != MULTIBOOT_MEMORY_AVAILABLE) continue;
        // make sure kernel is loaded at 0x100000 by bootloader(see linker.ld)
        if (mmap->addr_low == kmap->kernel.text_start_addr) {
            // set available memory starting from end of our kernel, leaving 1MB size for functions exceution
            kmap->available.start_addr = kmap->kernel.k_end_addr + 1024 * 1024;
            kmap->available.end_addr = mmap->addr_low + mmap->len_low;
            // get availabel memory in bytes
            kmap->available.size = kmap->available.end_addr - kmap->available.start_addr;
            return 0;
        }
    }

    return -1;
}

void display_kernel_memory_map(KERNEL_MEMORY_MAP *kmap) {
    printf("kernel:\n");
    printf("  kernel-start: 0x%x, kernel-end: 0x%x, TOTAL: %d bytes\n", 
            kmap->kernel.k_start_addr, kmap->kernel.k_end_addr, kmap->kernel.k_len);
    printf("  text-start: 0x%x, text-end: 0x%x, TOTAL: %d bytes\n", 
            kmap->kernel.text_start_addr, kmap->kernel.text_end_addr, kmap->kernel.text_len);
    printf("  data-start: 0x%x, data-end: 0x%x, TOTAL: %d bytes\n", 
            kmap->kernel.data_start_addr, kmap->kernel.data_end_addr, kmap->kernel.data_len);
    printf("  rodata-start: 0x%x, rodata-end: 0x%x, TOTAL: %d\n",
            kmap->kernel.rodata_start_addr, kmap->kernel.rodata_end_addr, kmap->kernel.rodata_len);
    printf("  bss-start: 0x%x, bss-end: 0x%x, TOTAL: %d\n",
            kmap->kernel.bss_start_addr, kmap->kernel.bss_end_addr, kmap->kernel.bss_len);

    printf("total_memory: %d KB\n", kmap->system.total_memory);
    printf("available:\n");
    printf("  start_adddr: 0x%x\n  end_addr: 0x%x\n  size: %d\n", 
            kmap->available.start_addr, kmap->available.end_addr, kmap->available.size);
}

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

void daemon(TIMER_FUNCTION function, uint32 timeout) {
    TIMER_FUNC_ARGS args = {0};
    args.timeout = timeout;
    timer_register_function(function, &args);
}

void pserial(const char* str) { // Use const char* for the string parameter
    write_serial(str);
    write_serial("\n");
}

void boot(unsigned long magic,  long addr) {

    MULTIBOOT_INFO *mboot_info;
    mboot_info = (MULTIBOOT_INFO *)addr;

    k_printf("\n\nCatKernel boot() started\n");

    // Initialize the file table
    for (size i = 0; i < MAX_FILES; ++i) {
        rootfs.file_table[i].filename[0] = '\0';  // Empty filename indicates an unused entry
    }
    k_printf("Starting sysdiag");
    sysdiaginit();
    k_printf("init_serial() on COM1");
    is_transmit_empty();
    init_serial();
    k_printf("\n                              Welcome to %CCat%CKernel\n", 0xB, 0x0, 0xE, 0x0);
    k_printf("Boot arguments: %s\n", args);
    //sleep(3)

    k_printf("Using standard VGA '%ux%u'\n", VGA_WIDTH, VGA_HEIGHT);
    k_printf("cpuid_info(1): Attempting to get CPU info");
    cpuid_info(1);
    k_printf("GetMemory(): Attempting to get Memory info");
    GetMemory();
    k_printf("Showing kernel's config.h configuration \\/ ");
    k_printf("   hostname: %s\n", host_name);
    pserial(host_name);
    k_printf("   username: %s\n", username);
    pserial(username);
    k_printf("   versionnumber: %s\n", versionnumber);
    pserial(versionnumber);
    k_printf("   versionname: %s\n", vername);
    pserial(vername);
    k_printf("   arch: %s\n", arch);
    pserial(arch);
    k_printf("   bootargs(default): %s\n", bootargs);
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
    k_printf("\n");

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

    kernmessage("Setting hostname to defaults from 'defaulthostname'");
    current_directory = "/etc";
    write_to_file(&rootfs, "hostname", defaulthostname);
    read_from_file(&rootfs, "hostname", buffer, sizeof(buffer), 1);

    gdt_init();
    idt_init();
    timer_init();

    kernmessage("Allocating memory for kernel...");

    // put the memory bitmap at the start of the available memory
    pmm_init(g_kmap.available.start_addr, g_kmap.available.size);
    pmm_init_region(g_kmap.available.start_addr, PMM_BLOCK_SIZE * 256);
    void *start = pmm_alloc_blocks(256);
    void *end = start + (pmm_next_free_frame(1) * PMM_BLOCK_SIZE);
    kheap_init(start, end);

    kernmessage("Allocated");

    memset(&g_kmap, 0, sizeof(KERNEL_MEMORY_MAP));

    get_kernel_memory_map(&g_kmap, mboot_info);

    //display_kernel_memory_map(&g_kmap);
    printf("total_memory: %d KB, %d Bytes\n", g_kmap.system.total_memory, g_kmap.available.size);
    printf("start_addr: 0x%x, end_addr: 0x%x\n", g_kmap.available.start_addr, g_kmap.available.end_addr);

    // put the memory bitmap at the start of the available memory
    pmm_init(g_kmap.available.start_addr, g_kmap.available.size);

    printf("Max blocks: %d\n", pmm_get_max_blocks());

    kernmessage("Setting hostname to defaults from 'defaulthostname'");
    current_directory = "/etc";
    write_to_file(&rootfs, "hostname", defaulthostname);
    read_from_file(&rootfs, "hostname", buffer, sizeof(buffer), 1);

    strcpy(host_name, buffer);
    kernmessage("Copying kernel values to proc");
    current_directory = "/proc";
    write_to_file(&rootfs, "arch", arch);
    k_printf("%C   Created /proc/arch\n", 0x8, 0x0);

    write_to_file(&rootfs, "args", bootargs);
    k_printf("%C   Created /proc/args\n", 0x8, 0x0);

    write_to_file(&rootfs, "version", vername);
    add_data_to_file(&rootfs, "version", versionnumber);
    k_printf("%C   Created /proc/version\n", 0x8, 0x0);

    write_to_file(&rootfs, "versionnum", versionnumber);
    k_printf("%C   Created /proc/versionnum\n", 0x8, 0x0);

    write_to_file(&rootfs, "ostype", "Catkernel");
    k_printf("%C   Created /proc/ostype\n", 0x8, 0x0);

    write_to_file(&rootfs, "cpu", brand);
    k_printf("%C   Created /proc/cpu\n", 0x8, 0x0);

    write_to_file(&rootfs, "vga", "80x25");
    k_printf("%C   Created /proc/vga\n", 0x8, 0x0);

    k_printf("Autoconfiguring devices...\n");
    BootDevConfig();

    kernmessage("Detecting ATA drives");
    //printf("\nExample\n");
    //const uint32 LBA = 0;
    //const uint8 NO_OF_SECTORS = 1;
    //char buf[ATA_SECTOR_SIZE] = {0};
    ata_init();

    daemon(launchp, 50);

    current_directory = "/boot";

    // root user
    kernmessage("Setting up default user");
    current_directory = "/etc";
    write_to_file(&rootfs, "users", "root\n");
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
    current_directory = "/boot";
    write_to_file(&rootfs, "compat-readme", "This folder is not used by CatK in any resonable way.\n\nThis is just here for UNIX compatibility :3");
    createhelpdocs();
    create_folder(&rootfs, "desktop", "/bin");
    current_directory = "/bin/desktop";
    write_to_file(&rootfs, "vga", "type:App\ngraphics_init");
    kernmessage("Created /sbin/vga");
    current_directory = "/etc";
    write_to_file(&rootfs, "issue", "Catkernel ");
    add_data_to_file(&rootfs, "issue", versionnumber);

    kernmessage("Setting up ENV variables");
    current_directory = "/var";
    write_to_file(&rootfs, "CATKVER", versionnumber);
    write_to_file(&rootfs, "HOSTNAME", "catk");
    write_to_file(&rootfs, "LANG", "en");
    write_to_file(&rootfs, "USER", "root");
    write_to_file(&rootfs, "HOME", "/home");
    write_to_file(&rootfs, "SHELL", "k_sh");

    pserial("Boot should be finished. Starting a shell");
}

void kmain(unsigned long magic, unsigned long addr) {
    MULTIBOOT_INFO *mboot_info;
    int i;
    mboot_info = (MULTIBOOT_INFO *)addr;
    console_init(COLOR_GREY, COLOR_BLACK);

    bootart();
    console_gotoxy(0, 0);

    k_printf("%CCat... %Ckernel!\n", 0x3, 0x0, 0xB, 0x0);
    k_printf("%CCatkernel %s booted with args: %s\n", 0xE, 0x0, versionnumber, (char *)mboot_info->cmdline);
    k_printf("%CLoader info \\/\n", 0x8, 0x0);

    k_printf("  magic: 0x%x\n", magic);
    k_printf("  flags: 0x%x\n", mboot_info->flags);
    k_printf("  mem_low: 0x%x KB\n", mboot_info->mem_low);
    k_printf("  mem_high: 0x%x KB\n", mboot_info->mem_high);
    k_printf("  boot_device: 0x%x\n", mboot_info->boot_device);
    k_printf("  cmdline: %s\n", (char *)mboot_info->cmdline);
    k_printf("  modules_count: %d\n", mboot_info->modules_count);
    k_printf("  modules_addr: 0x%x\n", mboot_info->modules_addr);
    k_printf("  mmap_length: %d\n", mboot_info->mmap_length);
    k_printf("  mmap_addr: 0x%x\n", mboot_info->mmap_addr);
    for (i = 0; i < mboot_info->mmap_length; i += sizeof(MULTIBOOT_MEMORY_MAP)) {
        MULTIBOOT_MEMORY_MAP *mmap = (MULTIBOOT_MEMORY_MAP *)(mboot_info->mmap_addr + i);
        k_printf("    size: %d, addr: 0x%x%x, len: %d%d, type: %d\n", 
                mmap->size, mmap->addr_low, mmap->addr_high, mmap->len_low, mmap->len_high, mmap->type);

        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
            /**** Available memory  ****/
        }
    }
    k_printf("  boot_loader_name: %s\n", (char *)mboot_info->boot_loader_name);
    k_printf("  vbe_control_info: 0x%x\n", mboot_info->vbe_control_info);
    k_printf("  vbe_mode_info: 0x%x", mboot_info->vbe_mode_info);
    // small delay to show the info
    for (int i = 0; i < 100000; ++i) {for (int j = 0; j < 1800; ++j) {}}
    boot(magic, addr);
}

void kernmessage(const char* str) { // Use const char* for the string parameter
    k_printf("kernel: %s\n", str); // Print the message and the string   
    write_serial(str);
    write_serial("\n");
    char* workingdirectory = current_directory;
    current_directory = "/etc";
    add_data_to_file(&rootfs, "logs", "\n");
    add_data_to_file(&rootfs, "logs", str);
    current_directory = workingdirectory;
}

void bootart()
{
        console_gotoxy(40, 3);
        printf("%C           __           __             ", 0xF, 0x0);
        console_gotoxy(40, 4);
        printf("%C          /  \\         /  \\        ", 0xF, 0x0);
        console_gotoxy(40, 5);
        printf("%C         / /\\ \\       / /\\ \\       ", 0xF, 0x0);
        console_gotoxy(40, 6);
        printf("%C        / /  \\ \\     / /  \\ \\      ", 0xB, 0x0);
        console_gotoxy(40, 7);
        printf("%C       / /      \\___/      \\ \\     ", 0xB, 0x0);
        console_gotoxy(40, 8);
        printf("%C      /                       \\    ", 0xB, 0x0);
        console_gotoxy(40, 9);
        printf("%C     |        |      |         |   ", 0xB, 0x0);
        console_gotoxy(40, 10);
        printf("%C   ---        |      |         --- ", 0xB, 0x0);
        console_gotoxy(40, 11);
        printf("%C     |                         |   ", 0xB, 0x0);
        console_gotoxy(40, 12);
        printf("%C   ---  ", 0x3, 0x0);
        printf("%C//", 0x3, 0x0);
        printf("%C       ^       ", 0x3, 0x0);
        printf("%C//", 0x3, 0x0);
        printf("%C    --- ", 0x3, 0x0);
        console_gotoxy(40, 13);
        printf("%C      \\         \\/\\/          /    ", 0xB, 0x0);
        console_gotoxy(40, 14);
        printf(" %C      \\                     /     ", 0xB, 0x0);
        console_gotoxy(40, 15);
        printf("%C        \\___________________/      ", 0x3, 0x0);
        console_gotoxy(40, 16);
        printf("%C         ===================       ", 0xC, 0x0);
        console_gotoxy(40, 17);
        printf("%C        =========", 0x4, 0x0);
        printf("%C\\/", 0xE,0x0);
        printf("%C==========      ", 0x4, 0x0);
        console_gotoxy(40, 18);
        printf("%C                /  \\               ", 0xE, 0x0);
        console_gotoxy(40, 19);
        printf("%C               |catk|              ", 0xE, 0x0);
        console_gotoxy(40, 20);
        printf("%C                \\__/               ", 0xE, 0x0);
}

/*
void catkmessagefixed(int NUM) { // Use const char* for the string parameter
   
    //do nothing
}*/