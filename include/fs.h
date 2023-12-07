#ifndef FS_H
#define FS_H

#include "types.h"

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
extern struct FileSystem rootfs;

static char buffer[BLOCK_SIZE];
static char buffer2[BLOCK_SIZE];

void rm_file(struct FileSystem* fs, const char* filename);
void create_folder(struct FileSystem* fs, const char* foldername, const char* parent_folder);
void cd_parent_directory();
void write_to_file(struct FileSystem* fs, const char* filename, const char* data, ...);
void list_files(const struct FileSystem* fs, int type);
void rm_file(struct FileSystem* fs, const char* filename);
void read_from_file(const struct FileSystem* fs, const char* filename, char* buffer, size buffer_size, int allow_anywhere);
void read_last_line_from_file(const struct FileSystem* fs, const char* filename, char* buffer, size buffer_size);
void add_data_to_file(struct FileSystem* fs, const char* filename, const char* additional_data);
void change_directory(struct FileSystem* fs, const char* path);
#endif