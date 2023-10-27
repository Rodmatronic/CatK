#include "kernel.h"
#include "console.h"
#include "string.h"
#include "io_ports.h"

// Keyboard I/O ports
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
// enter scancode
#define ENTER_KEY_SCANCODE 0x1C

// Define a variable to track the Shift key state
int shift_pressed = 0;

// Define a custom keymap for uppercase characters
static char uppercase_keymap[] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0, 'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.'
};

// Function to initialize the keyboard
void init_keyboard() {
    // Send initialization command to the keyboard
    outportb(KEYBOARD_STATUS_PORT, 0xAE); // Enable keyboard interrupts
}

// Function to read a key scancode from the keyboard
unsigned char read_key() {
    // Wait for the keyboard to have data available
    while (!(inportb(KEYBOARD_STATUS_PORT) & 0x01));

    // Read and return the scancode
    return inportb(KEYBOARD_DATA_PORT);
}

// Function to convert a scancode to a character
char scancode_to_char(unsigned char scancode) {
    // Define an array of characters for each scancode
    static char keymap[] = {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's',
        'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
        'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.'
    };

    if (scancode == 0x0E) {
        console_ungetchar();
        cursor_pos_x--;
    }else

    if (scancode == 0x48) { // Left Shift pressed
        scroll(1);
    }else
    if (scancode == 0xE0) { // Left Shift pressed
        scroll(0);
    }else

    if (scancode == 0x2A) { // Left Shift pressed
        shift_pressed = 1;
        printf("shift");
        return uppercase_keymap[scancode];
    } else if (scancode == 0xAA) { // Left Shift released
        shift_pressed = 0;
        printf("notpressedshift");
    }else

    // Check if the scancode is within the array bounds
    if (scancode < sizeof(keymap)) {
        return keymap[scancode];
    } else {
        return 0; // Unhandled scancode
    }
}