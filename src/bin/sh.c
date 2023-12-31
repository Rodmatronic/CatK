#include "console.h"
#include "vga.h"
#include "hostname.c"
#include "echo.c"
#include "gettime.c"
#include "ls.c"
#include "help.c"
#include "whoami.c"
#include "login.c"
#include "man.c"
#include "catsay.c"
#include "pkg.c"
#include "fs.h"
#include "string.h"
#include "config.h"

char* sessionbuffer[128];

void set_cursor_position(int x, int y);
void sh();
void process_user_input(const char* input);

int isclearing = 0;
int row = 3; // Track the current row
int stopglitchyhideingprompt = 0;
char* args = NULL; // Initialize args to NULL
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
    add_data_to_file(&rootfs, "logs.d", "sh: [ ok ] Started\n");
    startingrow = 0;
    row = 3;
    char input_buffer[80] = {0}; // Initialize with null characters to create an empty string

    console_clear(COLOR_WHITE, COLOR_BLACK);

    current_directory = "/etc";
    // Read and display the content of the specified file
    read_from_file(&rootfs, "motd", motdbuffer, sizeof(motdbuffer), 1);

    // Display the content
    printf("%s", motdbuffer);

    current_directory = "/";

    //printf_dark("You have been dropped into SH, type ");
    //printf("help");
    //printf_dark(" for some helpful commands");

    while (1) {
        isclearing = 0;
        console_gotoxy(0, row); // Set the cursor to the current row

        // Print the prompt with user, hostname, and current directory
        printf_brightcyan("%s", usersh);
        printf_darkcyan("@");
        printf_brightcyan("%s", hostnamesh);
        printf_brightblue(":%s ", current_directory);
        printf("# ");

        uint32 input_index = 0; // Index for the input buffer
        uint32 empty_input = 1; // Flag to track empty input
        uint32 can_backspace = 0; // Flag to allow or disallow backspacing
        uint32 cursorpos = 0; // Initialize cursor position

        int result = read_from_file(&rootfs, "tty", buffer, sizeof(buffer), 1);

        read_from_file(&rootfs, "tty", buffer, sizeof(buffer), 1);

        if (result == 2) {
            // Do something specific when the function returns 5
            panic("No TTY found!");
        }

        while (1) {

            unsigned char scancode = read_key();
            char key = scancode_to_char(scancode);

            if (cursorpos < 0) {
                can_backspace = 0;
                cursorpos = 0; // Ensure cursor position is non-negative
            }


            if (key != 0) {isclearing = 0;

                cursorpos++;

                if(cursorpos < 0)
                {
                    can_backspace = 0;
                }

                /*if (scancode == 0x2A){
                    // Assuming you have a FileSystem instance named rootfs
                    char last_entry[256]; // Adjust the buffer size as needed

                    // Read the last line from "history.ksh"
                    char* working_dir = current_directory;
                    current_directory = "/home";
                    read_last_line_from_file(&rootfs, "history.ksh", last_entry, sizeof(last_entry));
                    current_directory = working_dir;

                    // Print the last entry
                    printf("Last entry in history.ksh: %s\n", last_entry);
                }*/

                if (scancode == 0x0E && can_backspace == 1 && input_index > 0) {
                    cursorpos--;
                    input_index--;

                    // Remove the last character from the input buffer
                    input_buffer[input_index] = '\0';

                    // Update the cursor position
                    set_cursor_position(cursorpos, row);
                    
                    // Clear the character on the display
                    console_ungetchar();
                }
                
                if (scancode == ENTER_KEY_SCANCODE) {
                    if (!empty_input) {
                        printf("\n\n");
                        // Check if input length exceeds 20 characters
                        if (input_index >= 60) {
                            printf("Input too long. Please keep it under 60 characters.");
                            input_buffer[input_index] = '\0';
                            input_index = 0;
                            empty_input = 1;
                            can_backspace = 0; // Reset the backspace flag
        
                            for (int i = 0; i < 80; i++) {
                                input_buffer[i] = '\0';
                            }
                            row+=5;
                            if (row > 25)
                            {
                                row = 24;
                                printf("\n\n");
                            }
                            break;
                        }
                        stopglitchyhideingprompt = 1;
                        process_user_input(input_buffer);
                        if (stopglitchyhideingprompt == 1 && isclearing == 1)
                        {
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

    add_data_to_file(&rootfs, "logs.d", "sh: [ ok ] Processing user input...\n");
    args = NULL; // Reset the arguments

    char* working_dir = current_directory;
    current_directory = "/home";
    current_directory = rootUser.username;
    current_directory = working_dir;
    add_data_to_file(&rootfs, "history.ksh", input);
    add_data_to_file(&rootfs, "history.ksh", "\n");
    current_directory = current_directory;

    // Check for specific commands in the input
    if (string_starts_with(input, "pkg")) {
        args = input + strlen("pkg "); // Extract arguments
        // Check if there are arguments (non-empty)
        if (args[0] != '\0') {
            pkg(args);
        }
    }
    else if (string_starts_with(input, "echo")) {
        args = input + strlen("echo "); // Extract arguments
        // Check if there are arguments (non-empty)
        if (args[0] != '\0') {
                printf("%s", args);
                row++;
                return;
        }else
        printf("No string for CatK to say...");
    }
    
    if (string_starts_with(input, "init")) {
        init();
    }

     else if (string_starts_with(input, "hostname")) {
        args = input + strlen("hostname "); // Extract arguments
        // Check if there are arguments (non-empty)
        if (args[0] != '\0') {
            rm_file(&rootfs, "hostname");
            write_to_file(&rootfs, "hostname", args);
            read_from_file(&rootfs, "hostname", buffer, sizeof(buffer), 1);

            strcpy(host_name, buffer);

            current_directory = current_directory;
            if (row >= 25)
            {
                printf("\n");
                row = 26;
            }
        }
    } 
    else if (string_starts_with(input, "reboot")) {
        current_directory = "/sbin";
        execute_file(&rootfs, "reboot");
    }
    else if (string_starts_with(input, "halt")) {
        current_directory = "/sbin";
        execute_file(&rootfs, "halt");
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
        }else
        printf("No string to encrypt!");
    }
    else if (string_starts_with(input, "shutdown")) {
        current_directory = "/sbin";
        execute_file(&rootfs, "shutdown");
    }
    else if (string_starts_with(input, "poweroff")) {
        current_directory = "/sbin";
        execute_file(&rootfs, "shutdown");
    }
    else if (string_starts_with(input, "ls")) {
        args = input + strlen("ls "); // Extract arguments

        // Check if there are arguments (non-empty)
        if (args[0] != '\0') {
            if (strcmp(args, "-f") == 0)
            {
                list_files(&rootfs, 1);
                if (row >= 25)
                {
                    printf("\n");
                    row = 26;
                }
            }

            if (strcmp(args, "") == 0)
            {
                list_files(&rootfs, 0);
                row++;
                if (row >= 25)
                {
                    printf("\n");
                    row = 26;
                }
            }
        }else
            list_files(&rootfs, 0);
            row++;
            printf("\n\n");
            if (row >= 25)
            {
                printf("\n");
                row = 26;
            }
    }
    else if (string_starts_with(input, "exit")) {
        current_directory = "/bin";
        execute_file(&rootfs, "login");
    }
    else if (string_starts_with(input, "login")) {
        current_directory = "/bin";
        execute_file(&rootfs, "login");
    }
    else if (string_starts_with(input, "catsay")) {
        args = input + strlen("catsay "); // Extract arguments
        // Check if there are arguments (non-empty)
        if (args[0] != '\0') {
                catsay(args);
                row = 12;
        }else
        printf("No string for CatK to say...");
    }
    else if (string_starts_with(input, "cat")) {
        args = input + strlen("cat "); // Extract arguments
        // Check if there are arguments (non-empty)
        if (args[0] != '\0') {
                            // Read and display the content of the specified file
                read_from_file(&rootfs, args, buffer, sizeof(buffer), 0);

                // Display the content
                printf("%s\n", buffer);

                row+=25;
        }
    }
    else if (string_starts_with(input, "rm")) {
        args = input + strlen("rm "); // Extract arguments
        // Check if there are arguments (non-empty)
        if (args[0] != '\0') {
                rm_file(&rootfs, args);
                row++;
                if (row >= 25)
                {
                    printf("\n");
                    row = 26;
                }
        }
    }
else if (string_starts_with(input, "man")) {
    const char* man_command = input + strlen("man "); // Extract arguments
    // Check if there are arguments (non-empty)
    if (man_command[0] != '\0') {
        // Assign the extracted arguments to args without clearing
        args = man_command;

        // Remove any trailing whitespace or newline characters from args
        size args_len = strlen(args);
        while (args_len > 0 && (args[args_len - 1] == ' ' || args[args_len - 1] == '\n' || args[args_len - 1] == '\r')) {
            args[args_len - 1] = '\0';
            args_len--;
        }

        man(args);
        row += 25;
    }else
    printf("No such manual entry");
}

else if (string_starts_with(input, "touch")) {
    args = input + strlen("touch "); // Extract arguments
    
    // Check if there are arguments (non-empty)
    if (args[0] != '\0') {
        char filename[25]; // Assuming MAX_FILENAME_LENGTH is defined
        char contents[1024]; // Assuming MAX_CONTENTS_LENGTH is defined
        int argIndex = 0;
        
        // Skip leading spaces
        while (args[argIndex] == ' ') {
            argIndex++;
        }
        
        // Extract the filename
        int filenameIndex = 0;
        while (args[argIndex] != ' ' && args[argIndex] != '\0') {
            filename[filenameIndex++] = args[argIndex++];
        }
        filename[filenameIndex] = '\0';
        
        // Skip spaces between filename and contents
        while (args[argIndex] == ' ') {
            argIndex++;
        }
        
        // Extract the contents
        int contentsIndex = 0;
        while (args[argIndex] != '\0') {
            contents[contentsIndex++] = args[argIndex++];
        }
        contents[contentsIndex] = '\0';
        
        // Now, you can use filename and contents as needed
        write_to_file(&rootfs, filename, contents, current_directory);
    }
}

    else if (string_starts_with(input, "color")) {
        args = input + strlen("color "); // Extract arguments
        // Check if there are arguments (non-empty)
        if (args[0] != '\0') {
            printf("No such color %s\n", args);

            if (strcmp(args, "white") == 0) {  // Compare strings using strcmp
                console_init(COLOR_WHITE, COLOR_BLACK);
                row = -3;
            }
            if (strcmp(args, "black") == 0 || strcmp(args, "grey") == 0) {  // Compare strings using strcmp
                console_init(COLOR_DARK_GREY, COLOR_BLACK);
                row = -3;
            }
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
            if (strcmp(args, "pink") == 0) {  // Compare strings usSubscribeing strcmp
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
            current_directory = "/bin";
            execute_file(&rootfs, "uname");
        
    }   
    else if (string_starts_with(input, "mkdir")) {
        args = input + strlen("mkdir "); // Extract arguments
        // Check if there are arguments (non-empty)
        if (args[0] != '\0') {
                create_folder(&rootfs, args, current_directory);
                row++;
                if (row >= 25)
                {
                    printf("\n");
                    row = 26;
                }
        }
    }   
    else if (string_starts_with(input, "cd")) {
        args = input + strlen("cd "); // Extract arguments
        // Check if there are arguments (non-empty)
        if (args[0] != '\0') {
            if (strcmp(args, "..") == 0)
            {
                cd_parent_directory();
            }else
            change_directory(&rootfs, args);
            return;
        }
    }   
    else if (string_starts_with(input, "panic")) {
        args = input + strlen("panic "); // Extract arguments
        // Check if there are arguments (non-empty)
        if (args[0] != '\0') {
                panic(args);
        }else
            printf("No string for PANIC to display");
    }
    else if (string_starts_with(input, "exec")) {
        args = input + strlen("exec "); // Extract arguments
        // Check if there are arguments (non-empty)
        if (args[0] != '\0') {
            if (strcmp(args, "--help") == 0)
            {
                row += 8;
                printf("exec: [filename]\n");
                printf("    Temporarily replace the shell with the given command.\n\n");
                printf("    Execute COMMAND, replacing this shell with the specified program.\n");
                printf("    ARGUMENTS become the arguments to COMMAND.\n\n");
                printf("    Options:\n");
                printf("      n\\a\n");
                if (row >= 25)
                {
                    printf("\n");
                    row = 26;
                }
            }else
            if (strcmp(args, "sh") == 0)
            {
                printf("Cannot start SH from SH");
            }else
            if (args[0] != 'sh') {
                    execute_file(&rootfs, args);
                    row = 25;
            }
        }else
            printf("Please specify a file");
    }
    else if (string_starts_with(input, "help")) {
        help();
        row =+ 25;
        printf("\n");
    }
    else if (string_starts_with(input, "read")) {
        current_directory = "/bin";
        execute_file(&rootfs, "read");
    }
    else if (string_starts_with(input, "clear")) {
        isclearing = 1;
        row = -3;
        working_dir = current_directory;
        current_directory = "/bin";
        execute_file(&rootfs, "clear");
        current_directory = working_dir;
    } 
    else if (string_starts_with(input, "cpuid")) {
        read_from_file(&rootfs, "cpu", buffer, sizeof(buffer), 1);
        printf("%s", buffer);
    } 
    else if (string_starts_with(input, "whoami")) {
        read_from_file(&rootfs, "session.catk", buffer, sizeof(sessionbuffer), 1);
    }
    else if (string_starts_with(input, "times")) {
        printPowerOnTime();
    }
    else if (string_starts_with(input, "time")) {
        working_dir = current_directory;
        current_directory = "/sbin";
        execute_file(&rootfs, "time");
        current_directory = working_dir;
    }
else if (string_starts_with(input, "sleep")) {
    args = input + strlen("sleep "); // Extract arguments
    size args_len = strlen(args);

    // Check if there are arguments (non-empty)
    if (args[0] != '\0') {// Make sure there is at least one character in args (besides the newline)

        // Remove newline character from the end
        if (args[args_len - 1] == '\n') {
            args[args_len - 1] = '\0';
        }

        // Remove any trailing whitespace
        while (args_len > 0 && (args[args_len - 1] == '\r')) {
            args[args_len - 1] = '\0';
        }

        printf("Sleeping for %s seconds\n", args);
        sleep(atoi(args)); // Convert args to an integer and pass it to sleep
    } else {
        printf("Invalid sleep time\n");
    }
}
    else {
        add_data_to_file(&rootfs, "logs.d", "init: [ .. ] Command not found!\n");
        printf("%s: Command not found", input);
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
