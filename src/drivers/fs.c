#define MAX_FOLDERS 50
#define FOLDERNAME_SIZE 30

struct FileEntry {
    char filename[FILENAME_SIZE];
    uint32 start_block;
    uint32 size;
    uint8 is_folder;
    char parent_folder[FOLDERNAME_SIZE];
};

struct FileSystem {
    struct FileEntry file_table[MAX_FILES];
    char data_blocks[MAX_FILES][BLOCK_SIZE];
    char folder_table[MAX_FOLDERS][FOLDERNAME_SIZE];
};

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

            // Find an empty slot in the file table
            for (size j = 0; j < MAX_FILES; ++j) {
                if (fs->file_table[j].filename[0] == '\0' && fs->file_table[j].parent_folder[0] == '\0') {
                    // Empty slot found, create a new file entry for the folder
                    strncpy(fs->file_table[j].filename, foldername, FILENAME_SIZE);
                    fs->file_table[j].is_folder = 1;
                    strncpy(fs->file_table[j].parent_folder, parent_folder, FOLDERNAME_SIZE); // Set the parent folder
                    return;
                }
            }
            printf("Error: File table is full\n");
            return;
        }
    }
    printf("Error: Folder table is full\n");
}



void change_directory(struct FileSystem* fs, const char* foldername) {
    for (size i = 0; i < MAX_FOLDERS; ++i) {
        if (strcmp(fs->folder_table[i], foldername) == 0) {
            // Change the current working directory
            current_directory = fs->folder_table[i];
            printf("Changed directory to %s\n", foldername);
            return;
        }
    }
    printf("Error: Folder %s not found\n", foldername);
    // Reset the current directory to a safe default or handle it based on your design.
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
                    printf_brightblue("%s\tSize: %u bytes\n", fs->file_table[i].filename, fs->file_table[i].size);
                } else {
                    printf_green("%s\tSize: %u bytes\n", fs->file_table[i].filename, fs->file_table[i].size);
                }
                row++;
            }
        }
    }

    if (type == 0) {
        for (size i = 0; i < MAX_FILES; ++i) {
            if (fs->file_table[i].filename[0] != '\0' &&

                (strcmp(fs->file_table[i].parent_folder, current_folder) == 0 || strcmp(fs->file_table[i].parent_folder, current_directory) == 0)) {
                // Print in green if it's a folder, and in blue otherwise
                if (fs->file_table[i].is_folder) {
                    printf_brightblue("%s  ", fs->file_table[i].filename);
                } else {
                    printf_green("%s  ", fs->file_table[i].filename);
                }
            }
        }
    }
}

void read_from_file(const struct FileSystem* fs, const char* filename, char* buffer, size buffer_size) {
    for (size i = 0; i < MAX_FILES; ++i) {
        if (strcmp(fs->file_table[i].filename, filename) == 0) {
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

// Custom function to calculate the length of a string with a limit
size strnlen(const char* str, size max_len) {
    size len;
    for (len = 0; len < max_len && str[len] != '\0'; ++len);
    return len;
}

void rm_file(struct FileSystem* fs, const char* filename) {
    const char* current_folder = current_directory; // Get the current folder

    for (size i = 0; i < MAX_FILES; ++i) {
        if (strcmp(fs->file_table[i].filename, filename) == 0 &&
            strcmp(fs->file_table[i].parent_folder, current_folder) == 0) {
            
            // Check if the file entry is a folder
            if (fs->file_table[i].is_folder) {
                // Recursively remove contents of the folder
                remove_folder_contents(fs, fs->file_table[i].filename);
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
}



void execute_file(struct FileSystem* fs, const char* filename) {
    char buffer[BLOCK_SIZE];
    read_from_file(fs, filename, buffer, BLOCK_SIZE);

    // Tokenize and execute each line
    char* token = k_strtok(buffer, "\n");
    while (token != NULL) {
        // Run each line as a command
        // Note: You might need to implement your own command execution logic here
        printf("Executing: %s\n", token);

        // Move to the next line
        token = k_strtok(NULL, "\n");
    }
}

int k_strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        ++s1;
        ++s2;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}
int k_strtok(char* str, const char* delim) {
    static char* saved_ptr = NULL;

    if (str != NULL) {
        saved_ptr = str;
    } else if (saved_ptr == NULL) {
        return NULL; // No more tokens
    }

    // Find the start of the token
    char* token_start = saved_ptr;
    while (*saved_ptr != '\0' && *saved_ptr != *delim) {
        ++saved_ptr;
    }

    if (*saved_ptr == '\0') {
        saved_ptr = NULL; // No more tokens
    } else {
        *saved_ptr = '\0'; // Null-terminate the token
        ++saved_ptr; // Move to the next character after the delimiter
    }

    return token_start;
}
