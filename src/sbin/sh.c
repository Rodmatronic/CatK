#include "console.h"
#include "vga.h"
#include "read.h"
#include "syspw.h"
#include "panic.h"
#include "keyboard.h"
#include "string.h"

#define MAX_BUFFER_SIZE 128
char input_buffer[MAX_BUFFER_SIZE];
int buffer_index = 0;
int clearint = 0;

void execute_command(const char* command) {
    // Pre-built commands
    if (strcmp(command, "") == 0) {
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
    } else {
        // Default action for unrecognized commands
        printf("Unrecognized command.");
    }
}

void k_sh() {
    rows = 0;
    console_gotoxy(0, rows);
    printf("# ");

    while (1) {
        unsigned char scancode = read_key();

        if (scancode == 0x48) {
            return;
        }

        char key = scancode_to_char(scancode);

        if (key != 0) {
            if (scancode == ENTER_KEY_SCANCODE) {
                input_buffer[buffer_index] = '\0';
                execute_command(input_buffer);
                buffer_index = 0;
                
                if (rows >= 24) {
                    printf("\n");
                    rows = 24;
                } else
                    rows++;
                console_gotoxy(0, rows);
                printf("# ");
            } else {
                if (buffer_index < MAX_BUFFER_SIZE - 1) {
                    input_buffer[buffer_index++] = key;
                    printf("%c", key);
                }
            }
        }
    }
}
