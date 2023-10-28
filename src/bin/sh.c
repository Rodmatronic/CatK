#include "console.h"
#include "vga.h"
#include "hostname.c"
#include "echo.c"
#include "gettime.c"
#include "uname.c"
#include "ls.c"
#include "help.c"
#include "whoami.c"
#include "fs.h"
#include "string.h"

int row = 3; // Track the current row
char* args;

void sh() {

    row = 3;

    char input_buffer[80]; // Buffer to store user input

    console_clear(COLOR_WHITE, COLOR_BLACK);

    printf_dark("You have been dropped into SH, type ");
    printf("help");
    printf_dark(" for some helpful commands");

    while (1) {
        console_gotoxy(0, row); // Set the cursor to the current row
        printf("%s", username);
        printf("@");
        printf("%s", host_name);
        printf(":/ # ");

        int input_index = 0; // Index for the input buffer
        int empty_input = 1; // Flag to track empty input

        while (1) {
            // Read a key scancode
            unsigned char scancode = read_key();

            // Convert the scancode to a character
            char key = scancode_to_char(scancode);

            // Check if a valid character was returned
            if (key != 0) {
                if (scancode == 0x0E) {
                    input_index--; // Decrement the input_index
                    input_buffer[input_index] = '\0'; // Remove the character from the buffer
                }else

                // Print the character to the console
                if (scancode == ENTER_KEY_SCANCODE) {
                    // Check if the input is not empty before processing
                    if (!empty_input) {
                        printf("\n\n");
                        // Process the user's input (replace this with your command processing logic)
                        process_user_input(input_buffer);
                    }

                    // When Enter key is pressed, scancode 0x1C
                    if (row >= 24) {
                        printf("\n");
                        row = 24;
                    } else {
                        row += (empty_input ? 1 : 3); // Move down by 1 or 2 rows based on input
                    }
                    console_gotoxy(0, row); // Move to the next row

                    input_buffer[input_index] = '\0'; // Null-terminate the input
                    input_index = 0; // Reset the input index
                    empty_input = 1; // Reset the empty input flag

                    for (int i = 0; i < 80; i++) {
                        input_buffer[i] = '\0';
                    }
                    break; // Exit the inner loop
                } else {
                    // Store the character in the input buffer
                    input_buffer[input_index++] = key;
                    printf("%c", key); // Print the character to the console
                    empty_input = 0; // Mark input as non-empty
                }
            }
        }
    }
}

void process_user_input(const char* input) {
    args = NULL; // Reset the arguments

    // Check for specific commands in the input
    if (string_starts_with(input, "echo")) {
        // If "echo" command is detected, process it
        args = input + strlen("echo "); // Extract arguments
        // Check if there are arguments (non-empty)
        if (args[0] != '\0') {
            // Execute the "echo" command with the arguments
            echo(args);
        }
    } else if (string_starts_with(input, "hostname")) {
        args = input + strlen("hostname "); // Extract arguments
        // Check if there are arguments (non-empty)
        if (args[0] != '\0') {
            // Execute the "hostname" command with the arguments
            hostname(args);
        }
    } 
    else if (string_starts_with(input, "reboot")) {
        reboot();
    }
    else if (string_starts_with(input, "ls")) {
        ls(&root);
        row += 7;
    }
    else if (string_starts_with(input, "mkdir")) {
        args = input + strlen("mkdir "); // Extract arguments
        // Check if there are arguments (non-empty)
        if (args[0] != '\0') {
                mkdir(args);
        }
    }
    else if (string_starts_with(input, "touch")) {
        args = input + strlen("touch "); // Extract arguments
        // Check if there are arguments (non-empty)
        if (args[0] != '\0') {
                touch(args);
        }
    }
    else if (string_starts_with(input, "color")) {
        args = input + strlen("color "); // Extract arguments
        // Check if there are arguments (non-empty)
        if (args[0] != '\0') {
            printf("\nNo such color %s\n", args);

            if (strcmp(args, "red") == 0) {  // Compare strings using strcmp
                console_init(COLOR_BRIGHT_RED, COLOR_BLACK);
                row = 0;
            }
            if (strcmp(args, "green") == 0) {  // Compare strings using strcmp
                console_init(COLOR_BRIGHT_GREEN, COLOR_BLACK);
                row = 0;
            }
            if (strcmp(args, "blue") == 0) {  // Compare strings using strcmp
                console_init(COLOR_BRIGHT_BLUE, COLOR_BLACK);
                row = 0;
            }
            if (strcmp(args, "pink") == 0) {  // Compare strings using strcmp
                console_init(COLOR_BRIGHT_MAGENTA, COLOR_BLACK);
                row = 0;
            }
            if (strcmp(args, "cyan") == 0) {  // Compare strings using strcmp
                console_init(COLOR_CYAN, COLOR_BLACK);
                row = 0;
            }
        }
    }
    else if (string_starts_with(input, "uname")) {
        uname();
    }
    else if (string_starts_with(input, "panic")) {
        panic("Manually triggered via SH");
    }
    else if (string_starts_with(input, "help")) {
        help();
        row += 18;
    }
    else if (string_starts_with(input, "sh")) {
        sh();
    }
    else if (string_starts_with(input, "read")) {
        read();
    }
    else if (string_starts_with(input, "clear")) {
        row = -3;
        console_init(COLOR_WHITE, COLOR_BLACK);
    } 
    else if (string_starts_with(input, "cpuid")) {
        cpusimple(1);
    } 
    else if (string_starts_with(input, "whoami")) {
        whoami();
    } 
    else if (string_starts_with(input, "times")) {
        printPowerOnTime();
    } 
    else if (string_starts_with(input, "time")) {
        GetCurrentTime();
    }
    else {
        printf("Unknown command!");
    }
}

int string_starts_with(const char* str, const char* prefix) {
    // Utility function to check if a string starts with a specific prefix
    while (*prefix) {
        if (*prefix != *str) {
            return 0; // Mismatch found
        }
        prefix++;
        str++;
    }
    return 1; // Prefix found
}