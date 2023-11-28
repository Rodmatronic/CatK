#include "fs.h"
#include "libc.h"
#include "time.h"
#include "string.h"
#include "sleep.h"
#include "config.h"

#define BLOCK_SIZE 10000
#define FILENAME_SIZE 25
#define MAX_FILES 1024
#define MAX_FOLDERS 50
#define FOLDERNAME_SIZE 30

extern struct FileEntry {
    char filename[FILENAME_SIZE];
    uint32 start_block;
    uint32 size;
    uint8 is_folder;
    char parent_folder[FOLDERNAME_SIZE];
};


extern struct FileSystem {
    struct FileEntry file_table[MAX_FILES];
    char data_blocks[MAX_FILES][BLOCK_SIZE];
    char folder_table[MAX_FOLDERS][FOLDERNAME_SIZE];
};

// Create an instance of the filesystem
struct FileSystem rootfs;

void rm_file(struct FileSystem* fs, const char* filename);

void create_folder(struct FileSystem* fs, const char* foldername, const char* parent_folder) {
    // Check if the folder already exists
    for (size i = 0; i < MAX_FOLDERS; ++i) {
        if (strcmp(fs->folder_table[i], foldername) == 0 &&
            strcmp(fs->file_table[i].parent_folder, parent_folder) == 0) {
            printf("Error: Folder %s already exists\n", foldername);
            total_files++;
            return;
        }
    }

    // Find an empty slot in the folder table
    for (size i = 0; i < MAX_FOLDERS; ++i) {
        if (fs->folder_table[i][0] == '\0') {
            // Empty slot found, create a new folder entry
            strncpy(fs->folder_table[i], foldername, FOLDERNAME_SIZE);

            // Adjust the parent folder path for absolute paths
            char adjusted_parent[FOLDERNAME_SIZE];
            if (foldername[0] == '/') {
                strncpy(adjusted_parent, "/", FOLDERNAME_SIZE);
            } else {
                strncpy(adjusted_parent, parent_folder, FOLDERNAME_SIZE);
            }

            // Find an empty slot in the file table
            for (size j = 0; j < MAX_FILES; ++j) {
                if (fs->file_table[j].filename[0] == '\0' && fs->file_table[j].parent_folder[0] == '\0') {
                    // Empty slot found, create a new file entry for the folder
                    strncpy(fs->file_table[j].filename, foldername, FILENAME_SIZE);
                    fs->file_table[j].is_folder = 1;
                    strncpy(fs->file_table[j].parent_folder, adjusted_parent, FOLDERNAME_SIZE); // Set the parent folder
                    return;
                }
            }
            printf("Error: File table is full\n");
            return;
        }
    }
    printf("Error: Folder table is full\n");
}

void cd_parent_directory() {
    // Find the last occurrence of '/' in the current directory
    size last_slash = 0;
    for (size i = 0; i < FOLDERNAME_SIZE && current_directory[i] != '\0'; ++i) {
        if (current_directory[i] == '/') {
            last_slash = i;
        }
    }

    // Handle the case where we are in a subdirectory
    if (last_slash == 0) {
        current_directory[1] = '\0';  // Move back to root
    } else {
        current_directory[last_slash] = '\0';
    }

    //printf("Changed directory to %s\n", current_directory);
}

int snprintf(char *str, size size, const char *format, const char *arg, ...) {
    int result = 0;

    // Iterate through the format string and copy characters to the buffer
    while (*format && size > 1) {
        if (*format == '%' && *(format + 1) == 's') {
            // Handle %s format specifier
            while (*arg && size > 1) {
                *str++ = *arg++;
                size--;
                result++;
            }
            format += 2;  // Skip %s
        } else {
            *str++ = *format++;
            size--;
            result++;
        }
    }

    // Null-terminate the string
    if (size > 0) {
        *str = '\0';
    }

    return result;
}

void change_directory(struct FileSystem* fs, const char* path) {
    char new_path[FOLDERNAME_SIZE];

    if (path[0] == '/') {
        // Absolute path
        strncpy(new_path, path, FOLDERNAME_SIZE - 1);
        new_path[FOLDERNAME_SIZE - 1] = '\0';
    } else {
        // Relative path
        strncpy(new_path, current_directory, FOLDERNAME_SIZE - 1);
        new_path[FOLDERNAME_SIZE - 1] = '\0';

        // Ensure there is a '/' between the current directory and the relative path
        if (new_path[strlen(new_path) - 1] != '/' && strlen(new_path) < FOLDERNAME_SIZE - 1) {
            strncat(new_path, "/", 1);
        }

        strncat(new_path, path, FOLDERNAME_SIZE - strlen(new_path) - 1);
        new_path[FOLDERNAME_SIZE - 1] = '\0';
    }

    // Check if the folder exists
    int folder_found = 0;
    for (size i = 0; i < MAX_FOLDERS; ++i) {
        // Ensure the folder is an exact match and not just a substring
        if (strcmp(fs->folder_table[i], new_path) == 0) {
            // Set the flag to indicate folder found
            folder_found = 1;
            break;
        }
    }

    // Update the current working directory only if the folder is found
    if (folder_found) {
        strncpy(current_directory, new_path, FOLDERNAME_SIZE);
        //printf("Changed directory to %s\n", current_directory);
    } else {
        printf("Error: Folder %s not found\n", new_path);
        // Reset the current directory to a safe default or handle it based on your design.
    }
}


void write_to_file(struct FileSystem* fs, const char* filename, const char* data, ...) {
    const char* current_folder = current_directory; // Get the current folder

    // Check if the file already exists in the current folder
    for (size i = 0; i < MAX_FILES; ++i) {
        if (strcmp(fs->file_table[i].filename, filename) == 0 &&
            strcmp(fs->file_table[i].parent_folder, current_folder) == 0) {
            // File already exists, remove it
            rm_file(fs, filename);
            break; // Exit the loop after removing the file
        }
    }

    // Find an empty slot in the file table
    for (size i = 0; i < MAX_FILES; ++i) {
        if (fs->file_table[i].filename[0] == '\0' && fs->file_table[i].parent_folder[0] == '\0') {
            // Empty slot found, create a new entry in the current folder
            strncpy(fs->file_table[i].filename, filename, FILENAME_SIZE);
            strncpy(fs->data_blocks[i], data, BLOCK_SIZE);
            fs->file_table[i].start_block = i;
            fs->file_table[i].size = strlen(data);
            strncpy(fs->file_table[i].parent_folder, current_folder, FOLDERNAME_SIZE); // Set the parent folder
            total_files++;
            return;
        }
    }

    // File table is full
    printf("Error: File table is full\n");
}



void list_files(const struct FileSystem* fs, int type) {
    const char* current_folder = current_directory; // Get the current folder

    if (type == 1) {
        for (size i = 0; i < MAX_FILES; ++i) {
            if (fs->file_table[i].filename[0] != '\0' &&
                (strcmp(fs->file_table[i].parent_folder, current_folder) == 0 || strcmp(fs->file_table[i].parent_folder, current_directory) == 0)) {
                // Print in green if it's a folder, and in blue otherwise
                if (fs->file_table[i].is_folder) {
                    printf("%s\tSize: %u bytes\n", fs->file_table[i].filename, fs->file_table[i].size);
                } else {
                    printf("%s\tSize: %u bytes\n", fs->file_table[i].filename, fs->file_table[i].size);
                }
            }
        }
    }

    if (type == 0) {
        int line_length = 0;

        for (size i = 0; i < MAX_FILES; ++i) {
            if (fs->file_table[i].filename[0] != '\0' &&
                (strcmp(fs->file_table[i].parent_folder, current_folder) == 0 || strcmp(fs->file_table[i].parent_folder, current_directory) == 0)) {
                // Print in green if it's a folder, and in blue otherwise
                if (fs->file_table[i].is_folder) {
                    printf("%s  ", fs->file_table[i].filename);
                } else {
                    printf("%s  ", fs->file_table[i].filename);
                }

                line_length += strlen(fs->file_table[i].filename) + 2; // Account for filename and spaces

                if (line_length > VGA_WIDTH) {
                    printf("\n");
                    line_length = 0;
                }
            }
        }
    }
}

/*
// Recursive function to remove contents of a folder
void remove_folder_contents(struct FileSystem* fs, const char* foldername) {
    const char* current_folder = current_directory; // Get the current folder

    for (size i = 0; i < MAX_FILES; ++i) {
        if (fs->file_table[i].filename[0] != '\0' &&
            fs->file_table[i].is_folder &&
            strcmp(fs->file_table[i].parent_folder, foldername) == 0) {
            
            // Recursively remove contents of subfolders
            remove_folder_contents(fs, fs->file_table[i].filename);

            // Remove the file or folder entry
            fs->file_table[i].filename[0] = '\0';
            fs->file_table[i].start_block = 0;
            fs->file_table[i].size = 0;
            memset(fs->data_blocks[i], 0, BLOCK_SIZE); // Clear the data block
            total_files--;
        }
    }

    // Clear the folder entry itself
    for (size i = 0; i < MAX_FOLDERS; ++i) {
        if (strcmp(fs->folder_table[i], foldername) == 0) {
            fs->folder_table[i][0] = '\0';
            return;
        }
    }
}*/


void rm_file(struct FileSystem* fs, const char* filename) {
    const char* current_folder = current_directory; // Get the current folder

    for (size i = 0; i < MAX_FILES; ++i) {
        if (strcmp(fs->file_table[i].filename, filename) == 0 &&
            strcmp(fs->file_table[i].parent_folder, current_folder) == 0) {
            
            // Check if the file entry is a folder
            if (fs->file_table[i].is_folder) {
                // Recursively remove contents of the folder
               // remove_folder_contents(fs, fs->file_table[i].filename);
            }

            // Remove the file or folder entry
            fs->file_table[i].filename[0] = '\0';
            fs->file_table[i].start_block = 0;
            fs->file_table[i].size = 0;
            memset(fs->data_blocks[i], 0, BLOCK_SIZE); // Clear the data block
            total_files--;

            return;
        }
    }

    // File not found or not in the current folder
    printf("Error: File %s not found in the current folder\n", filename);
}

void read_from_file(const struct FileSystem* fs, const char* filename, char* buffer, size buffer_size, int allow_anywhere) {
    const char* current_folder = current_directory;

    size i;

    for (i = 0; i < MAX_FILES; ++i) {
        if (strcmp(fs->file_table[i].filename, filename) == 0) {
            // Check if the current folder matches the parent folder, unless explicitly allowed
            if (!allow_anywhere && strcmp(fs->file_table[i].parent_folder, current_folder) != 0) {
                printf("No such file in the current folder\n");
                strncpy(buffer, "", buffer_size); // Clear the buffer
                break;
            }

            // Check if the file is a folder
            if (fs->file_table[i].is_folder) {
                printf("%s is a folder and cannot be read as a file.\n", filename);
                strncpy(buffer, "", buffer_size); // Clear the buffer
                return;
            }

            // File found, read data from the corresponding data block
            strncpy(buffer, fs->data_blocks[fs->file_table[i].start_block], buffer_size);
            return;
        }
    }

    // File not found
    strncpy(buffer, "File not found", buffer_size);
}


void read_last_line_from_file(const struct FileSystem* fs, const char* filename, char* buffer, size buffer_size) {
    for (size i = 0; i < MAX_FILES; ++i) {
        if (strcmp(fs->file_table[i].filename, filename) == 0) {
            // Check if the file is a folder
            if (fs->file_table[i].is_folder) {
                printf("%s is a folder and cannot be read as a file.\n", filename);
                strncpy(buffer, "", buffer_size); // Clear the buffer
                return;
            }

            // File found, read data from the corresponding data block
            const char* file_data = fs->data_blocks[fs->file_table[i].start_block];

            // Find the last newline character in reverse
            size last_newline = -1;
            for (size j = strlen(file_data) - 1; j >= 0; --j) {
                if (file_data[j] == '\n') {
                    last_newline = j;
                    break;
                }
            }

            // Copy the last line into the buffer
            if (last_newline >= 0) {
                size last_line_length = strlen(&file_data[last_newline + 1]);
                strncpy(buffer, &file_data[last_newline + 1], last_line_length);
            } else {
                // No newline found, copy the entire file content
                strncpy(buffer, file_data, buffer_size);
            }
            return;
        }
    }

    // File not found
    strncpy(buffer, "File not found", buffer_size);
}

void add_data_to_file(struct FileSystem* fs, const char* filename, const char* additional_data) {
    for (size i = 0; i < MAX_FILES; ++i) {
        if (strcmp(fs->file_table[i].filename, filename) == 0) {
            // File found, append additional data to the corresponding data block

            // Calculate the remaining space in the block
            size remaining_space = BLOCK_SIZE - strlen(fs->data_blocks[fs->file_table[i].start_block]);

            // Check if there is enough space to add more data
            if (remaining_space > 0) {
                // Copy additional data, limited by the remaining space
                size len_to_copy = strnlen(additional_data, remaining_space);
                strncpy(fs->data_blocks[fs->file_table[i].start_block] + strlen(fs->data_blocks[fs->file_table[i].start_block]), additional_data, len_to_copy);

                // Update the file size
                fs->file_table[i].size += len_to_copy;
            } else {
                printf("Error: Not enough space to add more data to the file %s\n", filename);
            }
            
            return;
        }
    }

    // File not found
    printf("Error: File %s not found\n", filename);
}



void execute_file(struct FileSystem* fs, const char* filename) {
    char buffer[BLOCK_SIZE];
    read_from_file(fs, filename, buffer, BLOCK_SIZE, 0);

    // Check if the file is of type "app"
    char* typeToken = k_strstr(buffer, "type:App");
    if (typeToken == NULL) {
        // File is not of type "app", do not execute
        printf("Error: File is not of type 'app', or does not exist.\n");
        return;
    }
    // Tokenize and execute each line
    char* token = k_strtok(buffer, "\n");
    while (token != NULL) {
        // Check if the line contains "print "
        /*char* printToken = k_strstr(token, "print ");
        if (printToken != NULL) {
            // Print the text following "print "
            printf("%s\n", printToken + strlen("print "));
        }
        char* printTokendark = k_strstr(token, "print_dark ");
        if (printTokendark != NULL) {
            // Print the text following "print "
            printf_dark("%s\n", printTokendark + strlen("print_dark "));
        }
        char* delaycommand = k_strstr(token, "delay");
        if (delaycommand != NULL) {

            sleep(1);

        }
        char* clearToken = k_strstr(token, "clear");
        if (clearToken != NULL) {
            // Print the text following "clear"
            console_init(COLOR_WHITE, COLOR_BLACK);
        }
        char* Time = k_strstr(token, "time");
        if (Time != NULL) {
            GetCurrentTime();
        }
        char* Reboot = k_strstr(token, "reboot");
        if (Reboot != NULL) {
            syspw(0);
        }
        char* shutdown = k_strstr(token, "shutdown");
        if (shutdown != NULL) {
            syspw(1);
        }
        char* initcommand = k_strstr(token, "init");
        if (initcommand != NULL) {
            init();
        }
        char* shcommand = k_strstr(token, "sh");
        if (shcommand != NULL) {
            sh();
        }
        char* logincommand = k_strstr(token, "login");
        if (logincommand != NULL) {
            login();
        }
        char* version = k_strstr(token, "version");
        if (version != NULL) {
            read_from_file(&rootfs, "version", buffer, sizeof(buffer), 1);
            printf("%s", buffer);
        }
        char* readcommand = k_strstr(token, "read");
        if (readcommand != NULL) {
            read(0);
        }*/
        char* paniccommand = k_strstr(token, "panic");
        if (paniccommand != NULL) {
            panic("Triggered by Executable");
        }
        char* haltcommand = k_strstr(token, "halt");
        if (haltcommand != NULL) {
            syspw(2);
        }
        char* catasciilookupcommand = k_strstr(token, "catascii-lookup");
        if (catasciilookupcommand != NULL) {
            printf("                   _      _\n");
            printf("                  / \\    / \\\n");
            printf("                 /   \\__/   \\\n");
            printf("                /   |    |   \\\n");
            printf("               |      -       |\n");
            printf("              =|       o      |=\n");
            printf("              =\\              /=\n");
            printf("                \\            /\n");
            printf("                 =====\\/=====\n");
            printf("                    (CatK)\n");
        }
        char* catasciihappycommand = k_strstr(token, "catascii-happy");
        if (catasciihappycommand != NULL) {
            printf("                   _      _\n");
            printf("                  / \\    / \\\n");
            printf("                 /   \\__/   \\\n");
            printf("                /            \\\n");
            printf("               |    |    |    |\n");
            printf("              =|      -       |=\n");
            printf("              =\\      v       /=\n");
            printf("                \\            /\n");
            printf("                 =====\\/=====\n");
            printf("                    (CatK)\n");
        }
        char* catasciithinkcommand = k_strstr(token, "catascii-eh");
        if (catasciithinkcommand != NULL) {
            printf("                   _      _\n");
            printf("                  / \\    / \\\n");
            printf("                 /   \\__/   \\\n");
            printf("                /            \\\n");
            printf("               |     |    |   |\n");
            printf("              =|      -       |=\n");
            printf("              =\\      _       /=\n");
            printf("                \\            /\n");
            printf("                 =====\\/=====\n");
            printf("                    (CatK)\n");
        }
        char* catasciisleepcommand = k_strstr(token, "catascii-sleep");
        if (catasciisleepcommand != NULL) {
            printf("                   _      _\n");
            printf("                  / \\    / \\         z\n");
            printf("                 /   \\__/   \\     z\n");
            printf("                /            \\   z\n");
            printf("               |    _    _    |\n");
            printf("              =|      -       |=\n");
            printf("              =\\      o       /=\n");
            printf("                \\            /\n");
            printf("                 =====\\/=====\n");
            printf("                    (CatK)\n");
        }
        char* catasciisleepycommand = k_strstr(token, "catascii-tired");
        if (catasciisleepycommand != NULL) {
            printf("                   _      _\n");
            printf("                  / \\    / \\         z\n");
            printf("                 /   \\__/   \\     z\n");
            printf("                /            \\   z\n");
            printf("               |    _    _    |\n");
            printf("              =|      -       |=\n");
            printf("              =\\      w       /=\n");
            printf("                \\            /\n");
            printf("                 =====\\/=====\n");
            printf("                    (CatK)\n");
        }
        // Move to the next line
        token = k_strtok(NULL, "\n");
    }
}

void sync() {
    
}
