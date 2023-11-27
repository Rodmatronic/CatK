#ifndef FS_H
#define FS_H

#include "types.h"
#include "../src/drivers/fs.c"

#define BLOCK_SIZE 10000
#define FILENAME_SIZE 25
#define MAX_FILES 1024
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

// Create an instance of the filesystem
struct FileSystem rootfs;

char buffer[BLOCK_SIZE];

void execute_file(struct FileSystem* fs, const char* filename);

#endif