#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <term.h>
#include "printk.h"
#include "keyboard.h"
#include "k_sh.h"
#include "term.h"
#include "strcmp.h"
#include "power.h"
#include "config.h"

char shbuffer[128];
int shbuffer_index = 0;

void execute_command(const char* command) {
    if (command[0] == '\0') {
        return;
    }

    // Find the position of the first space character
    size_t space_index = 0;
    while (command[space_index] != ' ' && command[space_index] != '\0') {
        space_index++;
    }

    // Extract the command and parameter
    char extracted_command[space_index + 1];
    for (size_t i = 0; i < space_index; i++) {
        extracted_command[i] = command[i];
    }
    extracted_command[space_index] = '\0';

    if (strcmp(extracted_command, "uname") == 0) {
        if (command[space_index] == ' ') {
            space_index++;

            if (strcmp(&command[space_index], "-a") == 0) {
                printk("\n%s %s %s", sys_name, sys_ver, sys_arch);
            } else 
            if (strcmp(&command[space_index], "-m") == 0) {
                printk("\n%s", sys_arch);
            } else
            if (strcmp(&command[space_index], "-s") == 0) {
                printk("\n%s", sys_name);
            } else
            {
                // Unknown option
                printk("\nunknown option %s", &command[space_index]);
            }
        } else {
            printk("\n%s", sys_name);
    }
    } else 
    if (strcmp(extracted_command, "shutdown") == 0) {
        poweroff(1);
    } else 
    if (strcmp(extracted_command, "reboot") == 0) {
        poweroff(0);
    } else 
    if (strcmp(extracted_command, "halt") == 0) {
        poweroff(2);
    } else 
    if (strcmp(extracted_command, "debug") == 0) {
        printk("\n");
        printk("Arch: %s\n", sys_arch);
        printk("Term: %s\n", sys_term);
        printk("Sesh: %s\n", sys_sesh);
        printk("Vers: %s\n", sys_ver);
        printk("Home: %s\n", sys_home);
        printk("Name: %s\n", sys_name);
        printk("Mntp: %s\n", sys_mountpoint);

    } else 
    if (strcmp(extracted_command, "exit") == 0) {
        // Normally kill the SH on the terminal here, but... we dont have TTY nor processes
        printk("\nexit");
        return;
    } else 
    {
        // Command not found
        printk("\n%s: command not found", extracted_command);
    }
}

void k_sh() {
    printk("\n");
    printk("CatK built-in shell 0.01 -------\n----------------------\n\n");

    printk("%CK_sh", VGA_COLOR_CYAN);
    printk("%C# ", VGA_COLOR_LIGHT_GREY);
    
    while (1) {
        unsigned char scancode = read_key();
        char key = scancode_to_char(scancode);

        if (scancode == 0x0E)
        {
            shbuffer_index--;
            terminal_column--;
            terminal_putchar(0);
            vga_set_cursor_pos(terminal_column-=1, terminal_row);
        }else if (key != 0) {
            if (scancode == SCAN_CODE_KEY_ENTER) {
                // Handle Enter key press
                shbuffer[shbuffer_index] = '\0';
                execute_command(shbuffer);
                printk("\n");
                printk("%CK_sh", VGA_COLOR_CYAN);
                printk("%C# ", VGA_COLOR_LIGHT_GREY);
                shbuffer_index = 0;
            } else {
                shbuffer[shbuffer_index++] = key;
                printk("%c", key);
            }
        }
    }
}