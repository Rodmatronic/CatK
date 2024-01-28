#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "printk.h"
#include "keyboard.h"
#include "k_sh.h"
#include "term.h"
#include "strcmp.h"
#include "power.h"

char shbuffer[128];
int shbuffer_index = 0;

void execute_command(const char* command) {
    if (command[0] == '\0') {
        return;
    }
    if (strcmp(command, "uname") == 0) {
        printk("\nCatkernel");
    } else 
    if (strcmp(command, "shutdown") == 0) {
        poweroff(1);
    } else 
    if (strcmp(command, "reboot") == 0) {
        poweroff(0);
    } else 
    if (strcmp(command, "halt") == 0) {
        poweroff(2);
    } else 
    if (strcmp(command, "exit") == 0) {
        return;
    } else {
        printk("\n%s: command not found", command);
    }
}

void k_sh() {
    printk("\n");
    printk("CatK built-in shell 0.01-------\n\n");

    printk("K_sh# ");

    while (1) {
        unsigned char scancode = read_key();
        char key = scancode_to_char(scancode);

        if (scancode == 0x0E)
        {
            printk(" ");
            shbuffer_index--;
            terminal_column-=2;
            vga_set_cursor_pos(terminal_column, terminal_row);
        }else if (key != 0) {
            if (scancode == SCAN_CODE_KEY_ENTER) {
                // Handle Enter key press
                shbuffer[shbuffer_index] = '\0';
                execute_command(shbuffer);
                printk("\n");
                printk("K_sh# ");
                shbuffer_index = 0;
            } else {
                shbuffer[shbuffer_index++] = key;
                printk("%c", key);
            }
        }
    }
}