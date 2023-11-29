#include "console.h"
#include "vga.h"
#include "read.h"
#include "panic.h"
#include "read.h"
#include "keyboard.h"

void k_sh() {
    printf("\n");
    printf("# ");
    while (1) {
            // Read a key scancode
            unsigned char scancode = read_key();

            if (scancode == 0x48)
            {
                return;
            }


            // Convert the scancode to a character
            char key = scancode_to_char(scancode);

            // Check if a valid character was returned
            if (key != 0) {
                // Print the character to the console
                if (scancode == ENTER_KEY_SCANCODE) {
                    break; // Exit the inner loop
                }
                if (scancode == 0x48) {
                    console_scroll(1);
                }
                printf("%c", key);
            }
        
    }
}