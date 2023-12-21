#include "fs.h"
#include "console.h"
#include "keyboard.h"
#include "io_ports.h"
int ctrl_pressed;

void fwrite(char* name) {
    console_init(COLOR_WHITE, COLOR_BLACK);
    console_gotoxy(0, 0);
    printf("%C--------------------------------------------------------------------------------", 0xF, 0x0);
    console_gotoxy(0, 0);
    printf("%Cfwrite - %s\n", 0xF, 0x0, name);
    console_gotoxy(1, 22);
    printf("%C--------------------------------------------------------------------------------", 0xF, 0x0);
    console_gotoxy(1, 23);
    printf("%C^Q: Save and quit  ^S: Save", 0xF, 0x0);
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
            if (scancode == 0x1F && ctrl_pressed == 1) {
                // Write buffer contents to file
                write_to_file(&rootfs, filename, input_buffer);

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