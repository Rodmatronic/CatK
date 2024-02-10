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

void read(int anykey) {
    while (1) {
        unsigned char scancode = read_key();
        char key = scancode_to_char(scancode);

        if (scancode == 0x01) {
            break; // Exit the inner loop
        }

        if (scancode == 0x0E) {
            terminal_column--;
            terminal_putchar(0);
            vga_set_cursor_pos(terminal_column - 1, terminal_row);
        } else if (key != 0) {
            if (anykey == 1) {
                // If anykey is 1, return regardless of the key pressed
                return;
            }
            if (scancode == SCAN_CODE_KEY_ENTER) {
                printk("\n");
            } else {
                printk("%c", key);
            }
        }
    }
}