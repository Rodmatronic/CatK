#include "config.h"
#include "fs.h"
#include "console.h"
#include "vga.h"
#include "read.h"
#include "syspw.h"
#include "panic.h"
#include "keyboard.h"
#include "string.h"
#include "libc.h"
#include "time.h"
#include "exec.h"

#define MAX_BUFFER_SIZE 128
char input_buffer[MAX_BUFFER_SIZE];
int buffer_index = 0;
int clearint = 0;

void execute_command(const char* command) {
    // Pre-built commands
    if (strcmp(command, "") == 0) {
        // Do nothing for an empty command
    } else if (strcmp(command, "clear") == 0) {
        rows = 0;
        rows--;
        console_init(COLOR_WHITE, COLOR_BLACK);
    } else if (strcmp(command, "reboot") == 0) {
        rows = 0;
        syspw(0);
    } else if (strcmp(command, "shutdown") == 0) {
        rows = 0;
        syspw(1);
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
        printf("\n\n%s", new_directory);
        execute_file(&rootfs, new_directory, 0);
        vga_enable_cursor();
        rows+=25;
    } else if (strncmp(command, "rm ", 3) == 0) {
        const char* new_directory = command + 3;
        // Change to the specified directory
        printf("\n");
        rm_file(&rootfs, new_directory);
        rows+=2;
    } else if (strncmp(command, "time ", 4) == 0) {
        printf("\n\n");
        GetCurrentTime();
        rows+=2;
        
    }else if (strncmp(command, "cat ", 4) == 0) {
        const char* new_directory = command + 4;
        read_from_file(&rootfs, new_directory, buffer, sizeof(buffer), 0);
        printf("\n%s\n", buffer);
        rows+=25;
    } else if (strncmp(command, "uname", 4) == 0) {
        char* workingdir = current_directory;
        current_directory = "/proc";
        read_from_file(&rootfs, "version", buffer, sizeof(buffer), 0);
        printf("\n%s\n", buffer);
        rows+=2;
        current_directory = workingdir;
    } else if (strcmp(command, "ls") == 0) {
        printf("\n");
        list_files(&rootfs, 0);
        printf("\n");
        rows+=4;
    } else if (strncmp(command, "echo ", 5) == 0) {
        const char* new_directory = command + 5;
        // Change to the specified directory
        printf("\n\n%s", new_directory);
        rows+=2;
    } else if (strncmp(command, "cd ", 3) == 0) {
        // Check if the command starts with "cd "
        const char* new_directory = command + 3;  // Get the characters after "cd "
        if (strcmp(new_directory, "..") == 0) {
            cd_parent_directory();
        } else {
            // Change to the specified directory
            change_directory(&rootfs, new_directory);
        }
    } else {
        // Default action for unrecognized commands
        printf("\nUnrecognized command.\n");
        rows+=3;
    }
}
void PS1()
{
    console_gotoxy(0, rows);
    printf("%C[", 0xB, 0x0);
    printf("%s", 0xE, 0x0, username);
    printf("%C▓%s - %s (%s)", 0x3, 0x0, username, host_name, current_directory);
    printf("%C]%C# ", 0xB, 0x0, 0xF, 0x0);
}

void k_sh() {
    rows = 0;
    current_directory = "/";
    vga_enable_cursor();
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
