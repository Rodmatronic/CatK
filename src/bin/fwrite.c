#include "fs.h"
#include "console.h"
#include "keyboard.h"

int ctrl_pressed;

void fwrite(char* name) {
    console_init(COLOR_WHITE, COLOR_BLACK);
    // Original string with hyphens
    char* bar = "--------------------------------------------------------------------------";

    // Calculate the number of hyphens to remove based on the length of the name
    int hyphens_to_remove = strlen(name);

    // Ensure we don't remove more hyphens than the original string length
    hyphens_to_remove = (hyphens_to_remove < strlen(bar)) ? hyphens_to_remove : strlen(bar);

    // Calculate the new length of the bar string
    int new_length = strlen(bar) - hyphens_to_remove;

    // Create a new string without the trailing hyphens
    char new_bar[new_length + 1];  // +1 for the null terminator
    strncpy(new_bar, bar, new_length);
    new_bar[new_length] = '\0';  // Null-terminate the new string

    printf("%Cfwrite%s\n", 0xF, 0x0, new_bar);
    console_gotoxy(1, 22);
    printf("%C--------------------------------------------------------------------------------", 0xF, 0x0);
    console_gotoxy(1, 23);
    printf("%C^Q: Save and quit", 0xF, 0x0);
    console_gotoxy(0, 1);
    // Buffer to store input
    char input_buffer[1024];  // Adjust size as needed

    // Current position in the buffer
    int buffer_position = 0;

    char* filename = name;  // Name of the file to write

    // Main loop
    while (1) {
        // Read scancode
        unsigned char scancode = read_key();

        if (scancode == 0x1D) { // Left Shift pressed
            ctrl_pressed = 1;
        } else if (scancode == 0xAA) { // Left Shift released
            ctrl_pressed = 0;
        }

        // Convert the scancode to a character
        char key = scancode_to_char(scancode);

        // Check if a valid character was returned
        if (key != 0) {
            // Check for Enter to create a newline
            if (scancode == ENTER_KEY_SCANCODE) {
                // Add newline character to buffer
                input_buffer[buffer_position++] = '\n';
            }

            if (scancode == 0x10 && ctrl_pressed == 1) {
                
                // Write buffer contents to file
                write_to_file(&rootfs, filename, input_buffer);

                // Break out of the loop and exit
                break;
            }else
            if (scancode == SCAN_CODE_KEY_BACKSPACE) {
                if (buffer_position > 0) {
                    buffer_position--;
                    console_ungetchar();
                }
            }else
            {
            printf("%c", key);
            input_buffer[buffer_position++] = key;
            }
            
            if (buffer_position >= sizeof(input_buffer)) {
                // Write buffer contents to file
                write_to_file(&rootfs, filename, input_buffer);

                // Reset buffer position
                buffer_position = 0;
            }
        }
    }
}