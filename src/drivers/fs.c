

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

void list_files(const struct FileSystem* fs) {
    printf("Files in the filesystem:\n");

    for (size i = 0; i < MAX_FILES; ++i) {
        if (fs->file_table[i].filename[0] != '\0') {
            printf("%s\tSize: %u bytes\n", fs->file_table[i].filename, fs->file_table[i].size);
        }
    }
}
