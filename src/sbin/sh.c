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
int clearint = 0;

void execute_command(const char* command) {
    char* workingdir = current_directory;
    current_directory = "/home/root";
    add_data_to_file(&rootfs, "history", command);
    add_data_to_file(&rootfs, "history", "\n");
    current_directory = workingdir;

    // Pre-built commands
    if (strcmp(command, "top") == 0) {
        console_init(COLOR_WHITE, COLOR_BLACK);
        listProcesses();
        rows+=24;
    } else if (strcmp(command, "history") == 0) {
        char* workingdir = current_directory;
        current_directory = "/home/root";
        read_from_file(&rootfs, "history", buffer, sizeof(buffer), 0);
        printf("\n\n%s\n", buffer);
        rows+=24;
        current_directory = workingdir;
    } else if (strcmp(command, "") == 0) {
        // Do nothing for an empty command
    } else if (strcmp(command, "help") == 0) {
        char* working_dir = current_directory;
        console_init(COLOR_WHITE, COLOR_BLACK);
        rows+=24;
        current_directory = "/etc/docs";
        read_from_file(&rootfs, "helpdocs1", buffer, sizeof(buffer), 0);
        printf("%s", buffer);
        current_directory = working_dir;
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
        printf("\n%s\n", buffer);
        rows+=2;
        current_directory = workingdir;
    } else if (strncmp(command, "useradd ", 7) == 0)  {
        char* working_dir = current_directory;
        const char* newuser = command + 8;
        create_folder(&rootfs, newuser, "/home");
        current_directory = working_dir;
    } else if (strncmp(command, "hostname ", 8) == 0)  {
        char* working_dir = current_directory;
        const char* hostname2be_added = command + 9;
        current_directory = "/etc";
        write_to_file(&rootfs, "hostname", hostname2be_added);
        current_directory = working_dir;
    } else if (strcmp(command, "clear") == 0) {
        char* workingdir = current_directory;
        current_directory = "/bin";
        execute_file(&rootfs, "clear");
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
    } else if (strcmp(command, "halt") == 0) {
        rows = 0;
        syspw(2);
    } else if (strncmp(command, "panic", 6) == 0) {
        int numerator = 5;
        int divisor = 0;
        int result = numerator / divisor;
    } else if (strncmp(command, "exec ", 5) == 0) {
        const char* new_directory = command + 5;
        execute_file(&rootfs, new_directory);
        vga_enable_cursor();
    } else if (strncmp(command, "rm ", 3) == 0) {
        const char* new_directory = command + 3;
        rm_file(&rootfs, new_directory);
        rows+=1;
    } else if (strncmp(command, "about ", 5) == 0) {
        char* workingdir = current_directory;
        console_init(COLOR_WHITE, COLOR_BLACK);
        console_gotoxy(0, 0);
        printf("%CABOUT\n", 0xF, 0x0);
        console_gotoxy(4, 6);
        printf("%C             _      _\n", 0xF, 0x0);
        console_gotoxy(4, 7);
        printf("%C            / \\    / \\\n", 0xF, 0x0);
        console_gotoxy(4, 8);
        printf("%C           /   \\__/   \\\n", 0xB, 0x0);
        console_gotoxy(4, 9);
        printf("%C          /            \\\n", 0xB, 0x0);
        console_gotoxy(4, 10);
        printf("%C         |    |    |    |\n", 0xB, 0x0);
        console_gotoxy(4, 11);
        printf("%C        =|      -       |=\n", 0xB, 0x0);
        console_gotoxy(4, 12);
        printf("%C        =\\     \\/\\/     /=\n", 0xB, 0x0);
        console_gotoxy(4, 13);
        printf("%C          \\            /\n", 0x3, 0x0);
        console_gotoxy(4, 14);
        printf("%C           =====\\/=====\n", 0xC, 0x0);
        console_gotoxy(4, 15);
        printf("%C              (CatK)\n", 0xE, 0x0);
        console_gotoxy(40, 4);
        printf("%CThank you for using ", 0xF, 0x0);
        printf("%CCatkernel.\n", 0xB, 0x0);
        console_gotoxy(40, 5);
        printf("------------------------------\n");
        console_gotoxy(40, 6);
        current_directory = "/proc";
        read_from_file(&rootfs, "arch", buffer, sizeof(buffer), 0);
        printf("Arch: %s\n", buffer);
        console_gotoxy(40, 7);
        read_from_file(&rootfs, "ostype", buffer, sizeof(buffer), 0);
        printf("OS: %s\n", buffer);
        console_gotoxy(40, 8);
        current_directory = "/var";
        read_from_file(&rootfs, "SHELL", buffer, sizeof(buffer), 0);
        printf("Shell: %s\n", buffer);
        console_gotoxy(40, 9);
        printf("Dimensions: %ux%u\n", VGA_WIDTH, VGA_HEIGHT);
        console_gotoxy(40, 10);
        current_directory = "/proc";
        read_from_file(&rootfs, "cpu", buffer, sizeof(buffer), 0);
        printf("CPU: %s\n", buffer);
        console_gotoxy(40, 11);
        current_directory = "/var";
        read_from_file(&rootfs, "USER", buffer, sizeof(buffer), 0);
        printf("User: %s\n", buffer);
        console_gotoxy(40, 12);
        current_directory = "/var";
        read_from_file(&rootfs, "HOME", buffer, sizeof(buffer), 0);
        printf("Home: %s\n", buffer);

        current_directory = workingdir;
        rows+=24;
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
        printf("\n");
        if (rows >= 24)
        {
            printf("\n");
        }
        printf("%s\n", buffer);
        write_serial("uname: ");
        write_serial(buffer);
        write_serial("\n");
        rows+=1;
        current_directory = workingdir;
    } else if (strcmp(command, "ls") == 0) {
        printf("\n");
        if (rows >= 24)
        {
            printf("\n");
        }
        list_files(&rootfs, 0);
        write_serial("\n");
        rows+=2;
    } else if (strncmp(command, "serial ", 7) == 0) {
        const char* new_directory = command + 7;
        pserial(new_directory);
    } else if (strncmp(command, "echo ", 5) == 0) {
        const char* new_directory = command + 5;
        printf("\n");
        if (rows >= 24)
        {
            printf("\n");
        }
        printf("%s", new_directory);
        rows+=1;
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
    current_directory = "/etc";
    read_from_file(&rootfs, "session", buffer, sizeof(buffer), 1);
    read_from_file(&rootfs, "hostname", buffer2, sizeof(buffer2), 1);
    printf("%C▓%s - %s (%s)", 0x3, 0x0, buffer, buffer2, working_dir);
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
    current_directory = "/";
    PS1();
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