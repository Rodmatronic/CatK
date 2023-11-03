#include "console.h"
#include "vga.h"
#include "hostname.c"
#include "echo.c"
#include "gettime.c"
#include "uname.c"
#include "ls.c"
#include "help.c"
#include "whoami.c"
#include "login.c"
#include "fs.h"
#include "string.h"

int isclearing = 0;
int row = 3; // Track the current row
int stopglitchyhideingprompt = 0;
char* args;
uint8 startingrow;

// Function to set the cursor position
void set_cursor_position(int x, int y) {
    // The VGA hardware cursor position can be set through I/O ports 0x3D4 and 0x3D5
    // 0x3D4 is used to set the index (high byte or low byte) and 0x3D5 is used to set the data
    
    unsigned short position = (y * VGA_WIDTH) + x; // Calculate the position

    // Set the index to 0x0E (high byte) and write the high byte
    outportb(0x0E, 0x3D4);
    outportb((unsigned char)((position >> 8) & 0xFF), 0x3D5);

    // Set the index to 0x0F (low byte) and write the low byte
    outportb(0x0F, 0x3D4);
    outportb((unsigned char)(position & 0xFF), 0x3D5);
}

void sh() {
    startingrow = 0;
    row = 3;
    char input_buffer[80]; // Buffer to store user input

    console_clear(COLOR_WHITE, COLOR_BLACK);

    printf_dark("You have been dropped into SH, type ");
    printf("help");
    printf_dark(" for some helpful commands");

    while (1) {
        isclearing = 0;
        console_gotoxy(0, row); // Set the cursor to the current row
        printf("%s", username);
        printf("@");
        printf("%s", host_name);
        printf(":/ # ");

        int input_index = 0; // Index for the input buffer
        int empty_input = 1; // Flag to track empty input
        int can_backspace = 0; // Flag to allow or disallow backspacing
        int cursorpos;

        while (1) {
            unsigned char scancode = read_key();
            char key = scancode_to_char(scancode);

            if (cursorpos <= 0) {
                can_backspace = 0;
            }

            if (key != 0) {

                cursorpos++;

                if(cursorpos < 0)
                {
                    can_backspace = 0;
                }

                    if (scancode == 0x0E && can_backspace && input_index > 0) {
                        cursorpos--;
                        console_ungetchar();
                        input_index--;
                        
                        // Update the cursor position
                        set_cursor_position(cursorpos, row);
                    }
                
                if (scancode == ENTER_KEY_SCANCODE) {
                    if (!empty_input) {
                        printf("\n\n");
                        if (row == 3)
                        {
                           stopglitchyhideingprompt = 1;
                        }
                        process_user_input(input_buffer);
                        if (stopglitchyhideingprompt == 1 && isclearing == 0)
                        {
                            row = 24;
                            stopglitchyhideingprompt = 0;
                        }
                        if (isclearing == 0 && row >= 24)
                        {
                            row = 24;
                        }
                    }

                    if (row >= 24) {
                        printf("\n");
                        row = 24;
                    } else {
                        row += (empty_input ? 1 : 3);
                    }

                    console_gotoxy(0, row);

                    input_buffer[input_index] = '\0';
                    input_index = 0;
                    empty_input = 1;
                    can_backspace = 0; // Reset the backspace flag

                    for (int i = 0; i < 80; i++) {
                        input_buffer[i] = '\0';
                    }
                    break;
                } else {
                    if (scancode != 0x0E) {
                        input_buffer[input_index++] = key;
                        printf("%c", key);
                        empty_input = 0;
                        can_backspace = 1; // Allow backspacing when there's input
                    }
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
        syspw(0);
    }
    else if (string_starts_with(input, "passwd")) {
        args = input + strlen("passwd "); // Extract arguments
        // Check if there are arguments (non-empty)
        if (args[0] != '\0') {
            int args_length = strlen(args);

            char passwdinput[args_length + 1];  // Create a message array of the same size as args, plus 1 for null-termination
            strcpy(passwdinput, args);  // Copy the contents of args into message

            char encrypted[args_length + 1];  // Create a separate array for encrypted data, plus 1 for null-termination
            strcpy(encrypted, args);  // Copy the contents of args into encrypted
            encrypt(encrypted);
            encrypted[args_length] = '\0'; // Null-terminate the encrypted string
            
            printf("Encrypted message: %s\n", encrypted);
            decrypt(encrypted);
            printf("decrypted message: %s", encrypted);
            row += 2;
        }
    }
    else if (string_starts_with(input, "shutdown")) {
        syspw(1);
    }
    else if (string_starts_with(input, "ls")) {
        ls(&root);
        row += 7;
    }
    else if (string_starts_with(input, "theme")) {
        args = input + strlen("theme "); // Extract arguments
        // Check if there are arguments (non-empty)
        if (args[0] != '\0') {
            printf("\nNo such theme %s\n", args);

            if (strcmp(args, "ibm") == 0) {  // Compare strings using strcmp
                isclearing = 1;
                row = -3;
                console_init(COLOR_BRIGHT_GREEN, COLOR_BLACK);
            }
            if (strcmp(args, "solaris") == 0) {  // Compare strings using strcmp
                isclearing = 1;
                row = -3;
                console_init(COLOR_WHITE, COLOR_CYAN);
            }
            if (strcmp(args, "ubuntu") == 0) {  // Compare strings using strcmp
                console_init(COLOR_YELLOW, COLOR_MAGENTA);
                row = -3;
            }
            if (strcmp(args, "pink") == 0) {  // Compare strings using strcmp
                console_init(COLOR_BRIGHT_MAGENTA, COLOR_BLACK);
                row = -3;
            }
            if (strcmp(args, "cyan") == 0) {  // Compare strings using strcmp
                console_init(COLOR_CYAN, COLOR_BLACK);
                row = -3;
            }
        }
    } 
    else if (string_starts_with(input, "exit")) {
        login();
    }
    else if (string_starts_with(input, "login")) {
        login();
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
                row = -3;
            }
            if (strcmp(args, "green") == 0) {  // Compare strings using strcmp
                console_init(COLOR_BRIGHT_GREEN, COLOR_BLACK);
                row = -3;
            }
            if (strcmp(args, "blue") == 0) {  // Compare strings using strcmp
                console_init(COLOR_BRIGHT_BLUE, COLOR_BLACK);
                row = -3;
            }
            if (strcmp(args, "pink") == 0) {  // Compare strings using strcmp
                console_init(COLOR_BRIGHT_MAGENTA, COLOR_BLACK);
                row = -3;
            }
            if (strcmp(args, "cyan") == 0) {  // Compare strings using strcmp
                console_init(COLOR_CYAN, COLOR_BLACK);
                row = -3;
            }
        }
    }
    else if (string_starts_with(input, "uname")) {
        uname();
        printf("\n");
    }
    else if (string_starts_with(input, "panic")) {
        panic("Manually triggered via SH");
    }
    else if (string_starts_with(input, "help")) {
        help();
        row += 20;
    }
    else if (string_starts_with(input, "sh")) {
        sh();
    }
    else if (string_starts_with(input, "read")) {
        read();
    }
    else if (string_starts_with(input, "clear")) {
        isclearing = 1;
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
    else if (string_starts_with(input, "sleep")) {
        args = input + strlen("sleep "); // Extract arguments
        // Check if there are arguments (non-empty)
        if (args[0] != '\0') {
            args[strlen(args) - 1] = '\0'; // Null-terminate the arguments
            printf("%s", args);
            sleep(args);
            sh();
        }else{

        }
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