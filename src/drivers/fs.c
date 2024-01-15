#include "kernel.h"
#include "fs.h"
#include "console.h"
#include "libc.h"
#include "time.h"
#include "config.h"
#include "string.h"

#define BLOCK_SIZE 10000
#define FILENAME_SIZE 25
#define MAX_FILES 1024
#define MAX_FOLDERS 50
#define FOLDERNAME_SIZE 30

struct FileSystem rootfs;

void rm_file(struct FileSystem* fs, const char* filename);

void create_folder(struct FileSystem* fs, const char* foldername, const char* parent_folder) {
    // Check if the folder already exists
    for (size i = 0; i < MAX_FOLDERS; ++i) {
        if (strcmp(fs->folder_table[i], foldername) == 0 &&
            strcmp(fs->file_table[i].parent_folder, parent_folder) == 0) {
            printf("%C -error- Folder already exists\n", 0xF, 0x0);
            return;
        }
    }

    // Find an empty slot in the folder table
    for (size i = 0; i < MAX_FOLDERS; ++i) {
        if (fs->folder_table[i][0] == '\0') {
            // Empty slot found, create a new folder entry
            strncpy(fs->folder_table[i], foldername, FOLDERNAME_SIZE);

            // Adjust the parent folder path
            char adjusted_parent[FOLDERNAME_SIZE];
            if (parent_folder[0] == '/') {
                // Absolute path
                strncpy(adjusted_parent, parent_folder, FOLDERNAME_SIZE);
            } else {
                // Relative path
                strncpy(adjusted_parent, current_directory, FOLDERNAME_SIZE);
                if (adjusted_parent[strlen(adjusted_parent) - 1] != '/') {
                    strncat(adjusted_parent, "/", 1);
                }
                strncat(adjusted_parent, parent_folder, FOLDERNAME_SIZE - strlen(adjusted_parent) - 1);
            }

            // Find an empty slot in the file table
            for (size j = 0; j < MAX_FILES; ++j) {
                if (fs->file_table[j].filename[0] == '\0' && fs->file_table[j].parent_folder[0] == '\0') {
                    // Empty slot found, create a new file entry for the folder
                    strncpy(fs->file_table[j].filename, foldername, FILENAME_SIZE);
                    fs->file_table[j].is_folder = 1;
                    strncpy(fs->file_table[j].parent_folder, adjusted_parent, FOLDERNAME_SIZE);
                    return;
                }
            }
            printf("%C -error- File table is full\n", 0xF, 0x0);
            return;
        }
    }
    printf("%C -error- File table is full\n", 0xF, 0x0);
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

void change_directory(struct FileSystem* fs, const char* path) {
    char new_path[FOLDERNAME_SIZE];

    // Check if the path is "/"
    if (strcmp(path, "/") == 0) {
        current_directory = "/";
        return;
    }

    // Check if the path starts with "/"
    if (path[0] == '/') {
        current_directory = "/";
        return;
        // If the path starts with "/", set the current directory to root and then change to the specified folder
        strncpy(new_path, "/", FOLDERNAME_SIZE - 1);
        new_path[FOLDERNAME_SIZE - 1] = '\0';
        strncat(new_path, path + 1, FOLDERNAME_SIZE - strlen(new_path) - 1);
    } else {
        // Relative path
        strncpy(new_path, current_directory, FOLDERNAME_SIZE - 1);
        new_path[FOLDERNAME_SIZE - 1] = '\0';

        if (new_path[strlen(new_path) - 1] != '/') {
            strncat(new_path, "/", 1);
        }

        strncat(new_path, path, FOLDERNAME_SIZE - strlen(new_path) - 1);
        new_path[FOLDERNAME_SIZE - 1] = '\0';
    }

    // Check if the folder exists in the current folder or its subdirectories
    int folder_found = 0;
    for (size i = 0; i < MAX_FOLDERS; ++i) {
        if (strcmp(fs->folder_table[i], new_path) == 0 ||
            strcmp(fs->folder_table[i], path) == 0) {
            folder_found = 1;
            break;
        }
    }

    if (folder_found) {
        strncpy(current_directory, new_path, FOLDERNAME_SIZE);
        //printf("Changed directory to %s\n", current_directory);
    } else {
        printf("%C -error- Folder %s not found\n", 0xF, 0x0, new_path);
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
    printf("%C -error- File table is full\n", 0xF, 0x0);
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
        int row_count = 0;  // Track the number of rows

        for (size i = 0; i < MAX_FILES; ++i) {
            if (fs->file_table[i].filename[0] != '\0' &&
                (strcmp(fs->file_table[i].parent_folder, current_folder) == 0 || strcmp(fs->file_table[i].parent_folder, current_directory) == 0)) {
                // Print in green if it's a folder, and in blue otherwise
                if (fs->file_table[i].is_folder) {
                    printf("%C▓%s  ", 0xB, 0x0, fs->file_table[i].filename);
                    write_serial(fs->file_table[i].filename);
                    write_serial(" ");
                } else {
                    printf("%C▓%s  ", 0x9, 0x0, fs->file_table[i].filename);
                }

                line_length += strlen(fs->file_table[i].filename) + 2; // Account for filename and spaces

                if (line_length > VGA_WIDTH) {
                    printf("\n");
                    line_length = 0;
                    ++row_count;  // Increment row count
                }
            }
        }

        // Add ++ to rows
        //printf("\nNumber of Rows: %d\n", row_count);
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
            current_directory = current_folder;

            return;
        }
    }

    // File not found or not in the current folder
    printf("%C -error- No such file or directory\n", 0xF, 0x0);
}

void read_from_file(const struct FileSystem* fs, const char* filename, char* buffer, size buffer_size, int allow_anywhere) {
    const char* current_folder = current_directory;

    size i;

    for (i = 0; i < MAX_FILES; ++i) {
        if (strcmp(fs->file_table[i].filename, filename) == 0 &&
            strcmp(fs->file_table[i].parent_folder, current_folder) == 0) {
            // Check if the file is a folder
            if (fs->file_table[i].is_folder) {
                printf("%C -error- %s is a folder\n", 0xF, 0x0, filename);
                strncpy(buffer, "", buffer_size); // Clear the buffer
                return;
            }

            // File found, read data from the corresponding data block
            strncpy(buffer, fs->data_blocks[fs->file_table[i].start_block], buffer_size);
            return;
        }
    }

    // File not found or not in the current folder
    printf("%C -error- No such file or directory\n", 0xF, 0x0);
    strncpy(buffer, "", buffer_size); // Clear the buffer
}


void read_last_line_from_file(const struct FileSystem* fs, const char* filename, char* buffer, size buffer_size) {
    for (size i = 0; i < MAX_FILES; ++i) {
        if (strcmp(fs->file_table[i].filename, filename) == 0) {
            // Check if the file is a folder
            if (fs->file_table[i].is_folder) {
                printf("%C -error- %s is a folder\n", 0xF, 0x0, filename);
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
    printf("%C -error- No such file or directory\n", 0xF, 0x0);
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
                printf("%C -fatal I/O error- Not enough space to add more data to the file %s\n", 0xF, 0x0, filename);
            }
            
            return;
        }
    }

    // File not found
    printf("%C -error- No such file or directory\n", 0xF, 0x0);
}