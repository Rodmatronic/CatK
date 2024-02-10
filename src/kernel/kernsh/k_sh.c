#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <term.h>
#include <string.h>
#include "printk.h"
#include "keyboard.h"
#include "k_sh.h"
#include "term.h"
#include "power.h"
#include "config.h"
#include "panic.h"
#include "fsnode.h"
#include "time.h"
#include "read.h"
#include "stdlib.h"
#include "vfs.h"

int kill = 0;
char shbuffer[128];
int shbuffer_index = 0;
char shprompt[20]; 

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

    if (strcmp(extracted_command, "echo") == 0) {
        if (command[space_index] == ' ') {
            space_index++;
            printk("%s\n", &command[space_index]);
        } else {
            // No argument provided for echo
            printk("%s: missing argument\n", extracted_command);
        }
    } else
    if (strcmp(extracted_command, "prompt") == 0) {
        if (command[space_index] == ' ') {
            space_index++;
            const char* argument = &command[space_index];

            if (strlen(argument) < 20) {
                strcpy(shprompt, argument);
                printk("Updated prompt: %s\n", shprompt);
            } else {
                printk("%s: argument length exceeds 20 characters\n", extracted_command);
            }
        } else {
            // No argument provided for prompt
            printk("%s: missing argument\n", extracted_command);
        }
    }else
    if (strcmp(extracted_command, "uname") == 0) {
        if (command[space_index] == ' ') {
            space_index++;

            if (strcmp(&command[space_index], "-a") == 0) {
                printk("%s %s %s\n", sys_name, sys_ver, sys_arch);
            } else 
            if (strcmp(&command[space_index], "-m") == 0) {
                printk("%s\n", sys_arch);
            } else
            if (strcmp(&command[space_index], "-s") == 0) {
                printk("%s\n", sys_name);
            } else
            {
                // Unknown option
                printk("unknown option %s\n", &command[space_index]);
            }
        } else {
            printk("%s\n", sys_name);
    }
    } else 
    if (strcmp(extracted_command, "panic") == 0) {
        if (command[space_index] == ' ') {
            space_index++;

            if (strcmp(&command[space_index], "-d") == 0) {
                int zero = 0;
                int kill = zero / zero;
            } else 
            if (strcmp(&command[space_index], "-k") == 0) {
                kill = 1;
                return;
            } else 
            if (strcmp(&command[space_index], "-m") == 0) {
                asm("int     $0x10");
            } else
            if (strcmp(&command[space_index], "-b") == 0) {
                asm("int3");
            } else
            {
                // Unknown option
                printk("unknown option %s\n", &command[space_index]);
            }
        } else {
            panic("Generic SH error. (Specify using '-' next time!)");
    }
    } else 
    if (strcmp(extracted_command, "shutdown") == 0) {
        poweroff(1);
    } else 
    if (strcmp(extracted_command, "poweroff") == 0) {
        poweroff(1);
    } else 
    if (strcmp(extracted_command, "reboot") == 0) {
        poweroff(0);
    } else 
    if (strcmp(extracted_command, "halt") == 0) {
        poweroff(2);
    } else 
    if (strcmp(extracted_command, "clear") == 0) {
        terminal_clear();
    } else 
    if (strcmp(extracted_command, "mount") == 0) {
        displaynodes(fsnodes, nodecount);
    } else 
    if (strcmp(extracted_command, "exit") == 0) {
        printk("exit\n");
        return;
    } else 
    if (strcmp(extracted_command, "cmdline") == 0) {
        printk("%s\n", cmdline);
    } else 
    if (strcmp(extracted_command, "loader") == 0) {
        printk("%s\n", loader);
    } else 
    if (strcmp(extracted_command, "times") == 0) {
        printk("%d\n", counter);
    } else 
    // These commands are for some very simple POSIX complicancy. The FS is not usable currently, though.
    if (strcmp(extracted_command, "date") == 0) {
        current_full_date();
    } else 
    if (strcmp(extracted_command, "alias") == 0) {
    } else 
    if (strcmp(extracted_command, "logname") == 0) {
        printk("%s\n", sys_user);
    } else 
    if (strcmp(extracted_command, "lp") == 0) {
        printk("No printer device!\n");
    } else 
    if (strcmp(extracted_command, "read") == 0) {
        read();
    } else 
    if (strcmp(extracted_command, "test") == 0) {
    } else 
    if (strcmp(extracted_command, "cal") == 0) {
        cal();
    } else 
    if (strcmp(extracted_command, "ls") == 0) {
        v_listdir(sys_pwd);
    } else 
    if (strcmp(extracted_command, "mkdir") == 0) {
        //if (command[space_index] == ' ') {
        //    space_index++;
        //    v_createdir(&command[space_index]);
        //} else {
        //    printk("%s: missing argument\n", extracted_command);
        //}
    } else
    if (strcmp(extracted_command, "touch") == 0) {
        //if (command[space_index] == ' ') {
        //    space_index++;
        //    v_createfile(&command[space_index], sys_pwd);
        //} else {
        //    printk("%s: missing argument\n", extracted_command);
        //}
    } else
    if (strcmp(extracted_command, "sleep") == 0) {
        if (command[space_index] == ' ') {
            space_index++;
            const char* sleep_argument = &command[space_index];
            sleep(atoi(sleep_argument));
        } else {
            printk("%s: missing argument\n", extracted_command);
        }
        
    }else
    if (strcmp(extracted_command, "help") == 0) {
        printk("-- CatK and POSIX commands --\n");
        printk("alias\n");
        printk("cal\n");
        printk("clear\n");
        printk("cmdline\n");
        printk("date\n");
        printk("echo   [string]\n");
        printk("exit\n");
        printk("halt\n");
        printk("help\n");
        printk("logname\n");
        printk("lp\n");
        printk("loader\n");
        printk("mount\n");
        printk("panic   -d -k -m -b\n");
        printk("poweroff\n");
        printk("prompt   [string]\n");
        printk("read   [standard input]\n");
        printk("reboot\n");
        printk("shutdown\n");
        printk("sleep   [int]\n");
        printk("test\n");
        printk("times\n");
        printk("uname   -a -m -s\n");
    } else 
    {
        // Command not found
        printk("%s: command not found\n", extracted_command);
    }
}

void prompt()
{
    terminal_setcolor(VGA_COLOR_CYAN);
    printk("%s", shprompt);
    terminal_setcolor(VGA_COLOR_LIGHT_GREY);
    printk("%C# ", VGA_COLOR_LIGHT_GREY);
}

void k_sh() {
    prompt();
    while (1) {
        unsigned char scancode = read_key();
        char key = scancode_to_char(scancode);

        if (scancode == 0x0E)
        {
            if (shbuffer_index == 0)
            {
                printk(" ");
            }else
            shbuffer_index--;
            terminal_column--;
            terminal_putchar(0);
            vga_set_cursor_pos(terminal_column-=1, terminal_row);
        }else if (key != 0) {
            if (scancode == SCAN_CODE_KEY_ENTER) {
                // Handle Enter key press
                shbuffer[shbuffer_index] = '\0';
                printk("\n");
                execute_command(shbuffer);
                if (kill == 1)
                {
                    break;
                }
                prompt();
                shbuffer_index = 0;
            } else {
                shbuffer[shbuffer_index++] = key;
                printk("%c", key);
            }
        }
    }
}