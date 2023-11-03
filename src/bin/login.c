int strstr(const char *haystack, const char *needle) {
    int i, j;

    for (i = 0; haystack[i]; i++) {
        for (j = 0; needle[j]; j++) {
            if (haystack[i + j] != needle[j]) {
                break;
            }
        }
        if (!needle[j]) {
            return 1; // Found the substring
        }
    }

    return 0; // Substring not found
}

void login() {
    console_clear(COLOR_WHITE, COLOR_BLACK);
    char input_buffer[80]; // Buffer to store user input

    uname();
    printf(" (Terminal 1)\n\n");
    printf("Login: ");
    
    int input_index = 0;
    int found_root = 0; // Flag to track if "root" is found

    while (1) {
        // Read a key scancode
        unsigned char scancode = read_key();

        // Convert the scancode to a character
        char key = scancode_to_char(scancode);

        if (key != 0) {
            if (scancode == 0x0E) {
                // Handle backspace if needed
                if (input_index > 0) {
                    input_index--;
                    input_buffer[input_index] = '\0';

                    // Clear the last character on the screen
                    console_ungetchar();
                }
            } else {
                // Store the character in the input buffer
                input_buffer[input_index++] = key;
                input_buffer[input_index] = '\0'; // Null-terminate the input
                
                // Print the character to the console
                printf("%c", key);
                
                // Check if "root" is found in the input
                if (strstr(input_buffer, "root") != NULL) {
                    found_root = 1;
                }
            }
        }

        // Check if Enter key is pressed (0x1C)
        if (scancode == ENTER_KEY_SCANCODE) {
            printf("\n");

            if (found_root) {
                strcpy(userid, "root");
                strcpy(username, "root");
                sh();
            } else {
                login();
            }
        }
    }
}
