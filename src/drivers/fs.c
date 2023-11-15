

struct FileEntry {
    char filename[FILENAME_SIZE];
    uint32 start_block;
    uint32 size;
};

struct FileSystem {
    struct FileEntry file_table[MAX_FILES];
    char data_blocks[MAX_FILES][BLOCK_SIZE];
};

void write_to_file(struct FileSystem* fs, const char* filename, const char* data) {
    for (size i = 0; i < MAX_FILES; ++i) {
        if (strcmp(fs->file_table[i].filename, filename) == 0) {
            // File found, write data to the corresponding data block
            strncpy(fs->data_blocks[fs->file_table[i].start_block], data, BLOCK_SIZE);
            
            // Update the file size
            fs->file_table[i].size = strlen(data);
            return;
        }
    }

    // File not found, find an empty slot in the file table
    for (size i = 0; i < MAX_FILES; ++i) {
        if (fs->file_table[i].filename[0] == '\0') {
            // Empty slot found, create a new entry
            strncpy(fs->file_table[i].filename, filename, FILENAME_SIZE);
            strncpy(fs->data_blocks[i], data, BLOCK_SIZE);
            fs->file_table[i].start_block = i;
            fs->file_table[i].size = strlen(data);
            return;
        }
    }
}

void list_files(const struct FileSystem* fs, int type) {

    if (type == 1)
    {
            for (size i = 0; i < MAX_FILES; ++i) {
            if (fs->file_table[i].filename[0] != '\0') {
                printf("%s\tSize: %u bytes\n", fs->file_table[i].filename, fs->file_table[i].size);
                row+=1;
            }
        }
    }

    if (type == 0)
    {
            for (size i = 0; i < MAX_FILES; ++i) {
            if (fs->file_table[i].filename[0] != '\0') {
                printf("%s  ", fs->file_table[i].filename);
            }
        }
    }
}

void read_from_file(const struct FileSystem* fs, const char* filename, char* buffer, size buffer_size) {
    for (size i = 0; i < MAX_FILES; ++i) {
        if (strcmp(fs->file_table[i].filename, filename) == 0) {
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
    for (size i = 0; i < MAX_FILES; ++i) {
        if (strcmp(fs->file_table[i].filename, filename) == 0) {
            // File found, remove it by marking the entry as empty
            fs->file_table[i].filename[0] = '\0';
            fs->file_table[i].start_block = 0;
            fs->file_table[i].size = 0;
            memset(fs->data_blocks[i], 0, BLOCK_SIZE); // Clear the data block
            return;
        }
    }

    // File not found
    printf("Error: File %s not found\n", filename);
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