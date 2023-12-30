#include "config.h"
#include "kernel.h"
#include "fs.h"
#include "console.h"
#include "vga.h"
#include "read.h"
#include "syspw.h"
#include "panic.h"
#include "keyboard.h"
#include "string.h"
#include "libc.h"
#include "ide.h"
#include "time.h"
#include "exec.h"
#include "fwrite.h"
#include "process.h"

#define MAX_BUFFER_SIZE 128
char input_buffer[MAX_BUFFER_SIZE];
int buffer_index = 0;
int clearint = 0;

void execute_command(const char* command) {
    // Pre-built commands
    if (strcmp(command, "top") == 0) {
        console_init(COLOR_WHITE, COLOR_BLACK);
        listProcesses();
        rows+=24;

    } else if (strcmp(command, "") == 0) {
        // Do nothing for an empty command
    } else if (strcmp(command, "help") == 0) {
        printf("\n%CCatK %s k_sh help\n\n", 0xB, 0x0, versionnumber);
        printf("cd - [dir]        echo - [input]\n");
        printf("ls - [input]      halt - n/a\n");
        printf("rm - [input]      time - n/a\n");
        printf("reboot - n/a      cat  - [input]\n");
        printf("shutdown - n/a    help - n/a\n");
        printf("halt - n/a        exec - [input]\n");
        printf("whoami - n/a      hostname - [input]\n");
        printf("dmesg - n/a       uname - n/a\n");
        printf("about - n/a       panic - [input]\n");
        rows+=12;

    } else if (strcmp(command, "dmesg") == 0) {
        char* workingdir = current_directory;
        current_directory = "/etc";
        read_from_file(&rootfs, "logs", buffer, sizeof(buffer), 0);
        printf("\n\n%s\n", buffer);
        rows+=24;
        current_directory = workingdir;
    } else if (strcmp(command, "whoami") == 0) {
        char* workingdir = current_directory;
        current_directory = "/etc";
        read_from_file(&rootfs, "session", buffer, sizeof(buffer), 0);
        printf("\n\n%s\n", buffer);
        rows+=3;
        current_directory = workingdir;
    } else if (strncmp(command, "hostname ", 8) == 0)  {
        char* working_dir = current_directory;
        const char* hostname2be_added = command + 9;
        current_directory = "/etc";
        write_to_file(&rootfs, "hostname", hostname2be_added);
        current_directory = working_dir;
    } else if (strcmp(command, "clear") == 0) {
        char* workingdir = current_directory;
        current_directory = "/bin";
        execute_file(&rootfs, "clear", 1);
        current_directory = workingdir;
    } else if (strcmp(command, "reboot") == 0) {
        rows = 0;
        syspw(0);
    } else if (strcmp(command, "shutdown") == 0) {
        rows = 0;
        syspw(1);
    } else if (strncmp(command, "fwrite ", 7) == 0) {
        const char* new_directory = command + 7;
        fwrite(new_directory);
        rows+=24;
    } else if (strcmp(command, "fwrite") == 0) {
        printf("%C --Please specify a filename--", 0xF, 0x0);
    } else if (strncmp(command, "mkdir ", 6) == 0) {
        const char* new_directory = command + 6;
        char* workingdir = current_directory;
        create_folder(&rootfs, new_directory, workingdir);
        
        rows+=24;
    } else if (strcmp(command, "halt") == 0) {
        rows = 0;
        syspw(2);
    } else if (strncmp(command, "panic ", 6) == 0) {
        const char* new_directory = command + 6;
        // Change to the specified directory
        panic(new_directory);
    } else if (strncmp(command, "exec ", 5) == 0) {
        const char* new_directory = command + 5;
        // Change to the specified directory
        execute_file(&rootfs, new_directory, 0);
        vga_enable_cursor();
        rows+=25;
    } else if (strncmp(command, "rm ", 3) == 0) {
        const char* new_directory = command + 3;
        // Change to the specified directory
        printf("\n");
        rm_file(&rootfs, new_directory);
        rows+=2;
    } else if (strncmp(command, "about ", 5) == 0) {
        char* workingdir = current_directory;
        printf("\n\n");
        printf("Current shell is k_sh, running on ");
        current_directory = "/proc";
        read_from_file(&rootfs, "arch", buffer, sizeof(buffer), 0);
        printf("%s", buffer);
        printf(". Thank you for using Catkernel.\n");
        if (rows > 24)
        {
            rows+=2;
        }
        current_directory = workingdir;
        rows+=3;
    }else if (strncmp(command, "time ", 4) == 0) {
        printf("\n\n");
        GetCurrentTime();
        rows+=2;
    }else if (strncmp(command, "cat ", 4) == 0) {
        console_init(COLOR_WHITE, COLOR_BLACK);
        const char* new_directory = command + 4;
        printf("%CCat-----------------------------------------------------------------------------\n", 0xF, 0x0);
        read_from_file(&rootfs, new_directory, buffer, sizeof(buffer), 0);
        printf("%s\n", buffer);
        rows+=24;
    } else if (strncmp(command, "dd ", 3) == 0) {
        char* workingdir = current_directory;
        console_init(COLOR_WHITE, COLOR_BLACK);
        printf("WARNING. THIS WILL WRITE ANY DATA PROVIDED TO THE FIRST DISK AVALIBLE!\n", 0x8, 0x0);
        printf("PLEASE %CREMOVE", 0x4, 0x0);
        printf(" ANY DISKS YOU CARE ABOUT!");
        read(0);
        printf("THIS IS NOT A JOKE! THIS IS AN IN DEVELOPMENT FEATURE!");
        read(0);
        printf("PLEASE %CREMOVE", 0x4, 0x0);
        printf(" ANY DISKS YOU CARE ABOUT!\n");
        read(0);
        const int DRIVE = ata_get_drive_by_model("VBOX HARDDISK");
        const uint32 LBA = 0;
        const uint8 NO_OF_SECTORS = 1;
        char buf[ATA_SECTOR_SIZE] = {0};

        const char* datatorw = command + 3;
        struct data {
            int id;
            char name[32];
        };

        struct data e;
        e.id = 10012;
        strcpy(e.name, datatorw);

        // write message to drive
        strcpy(buf, datatorw);
        ide_write_sectors(DRIVE, NO_OF_SECTORS, LBA, (uint32)buf);

        memset(buf, 0, sizeof(buf));
        memcpy(buf, &e, sizeof(e));
        ide_write_sectors(DRIVE, NO_OF_SECTORS, LBA + 1, (uint32)buf);
        printf("data written\n");

        // read message from drive
        memset(buf, 0, sizeof(buf));
        ide_read_sectors(DRIVE, NO_OF_SECTORS, LBA, (uint32)buf);
        printf("read data: %s\n", buf);

        memset(buf, 0, sizeof(buf));
        ide_read_sectors(DRIVE, NO_OF_SECTORS, LBA + 1, (uint32)buf);
        memcpy(&e, buf, sizeof(e));
        printf("id: %d, name: %s\n", e.id, e.name);

        rows += 23;

        current_directory = workingdir;
    } else if (strncmp(command, "uname", 4) == 0) {
        char* workingdir = current_directory;
        current_directory = "/proc";
        read_from_file(&rootfs, "version", buffer, sizeof(buffer), 0);
        printf("\n%s\n", buffer);
        write_serial("uname: ");
        write_serial(buffer);
        write_serial("\n");
        rows+=2;
        current_directory = workingdir;
    } else if (strcmp(command, "ls") == 0) {
        printf("\n\n");
        list_files(&rootfs, 0);
        write_serial("\n");
        rows+=4;
    } else if (strncmp(command, "serial ", 7) == 0) {
        const char* new_directory = command + 7;
        pserial(new_directory);
    } else if (strncmp(command, "echo ", 5) == 0) {
        const char* new_directory = command + 5;
        printf("\n\n%s", new_directory);
        rows+=3;
    } else if (strncmp(command, "cd ", 3) == 0) {
        // Check if the command starts with "cd "
        const char* new_directory = command + 3;  // Get the characters after "cd "
        if (strcmp(new_directory, "..") == 0) {
            cd_parent_directory();
            pserial("cd to ..");
        } else {
            // Change to the specified directory
            change_directory(&rootfs, new_directory);
            write_serial("cd to ");
            write_serial(new_directory);
            write_serial("\n");
            new_directory = 0;
        }
    } else {
        // Default action for unrecognized commands
        printf("%C -error- Unrecognized command.\n", 0xF, 0x0);
    }
}


void PS1()
{
    pserial("Setting PS1 for k_sh");
    char* working_dir = current_directory;
    console_gotoxy(0, rows);
    printf("%C[", 0xB, 0x0);
    read_from_file(&rootfs, "session", buffer, sizeof(buffer), 1);
    read_from_file(&rootfs, "hostname", buffer2, sizeof(buffer2), 1);
    printf("%C▓%s - %s (%s)", 0x3, 0x0, buffer, buffer2, current_directory);
    printf("%C]%C# ", 0xB, 0x0, 0xF, 0x0);
    current_directory = working_dir;
}

void motd()
{
    char* working_dir = current_directory;
    current_directory = "/etc";
    read_from_file(&rootfs, "motd", buffer, sizeof(buffer), 1);
    printf("%s", buffer);
    current_directory = working_dir;
}

void k_sh() {
    console_init(COLOR_WHITE, COLOR_BLACK);
    write_serial("k_sh for catk version ");
    write_serial(versionnumber);
    write_serial("\n");
    rows = 3;
    vga_enable_cursor();
    pserial("showing motd");
    motd();
    pserial("showing PS1");
    PS1();
    current_directory = "/";
    while (1) {
        unsigned char scancode = read_key();
        char key = scancode_to_char(scancode);

        if (key != 0) {
            if (scancode == SCAN_CODE_KEY_BACKSPACE) {
                if (buffer_index > 0) {
                    buffer_index--;
                    console_ungetchar();
                }
            } else if (scancode == ENTER_KEY_SCANCODE) {
                input_buffer[buffer_index] = '\0';
                execute_command(input_buffer);
                buffer_index = 0;
                
                if (rows >= 24) {
                    printf("\n");
                    rows = 24;
                } else
                    rows++;
                console_gotoxy(0, rows);
                PS1();
            } else {
                if (buffer_index < MAX_BUFFER_SIZE - 1) {
                    input_buffer[buffer_index++] = key;
                    printf("%c", key);
                }
            }
        }
    }
}