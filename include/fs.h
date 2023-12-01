#ifndef FS_H
#define FS_H

#include "types.h"
#include "../src/drivers/fs.c"

#define BLOCK_SIZE 10000
#define FILENAME_SIZE 25
#define MAX_FILES 1024
#define MAX_FOLDERS 50
#define FOLDERNAME_SIZE 30

// Create an instance of the filesystem
struct FileSystem rootfs;

char buffer[BLOCK_SIZE];

void create_folder(struct FileSystem* fs, const char* foldername, const char* parent_folder);
void rm_file(struct FileSystem* fs, const char* filename);
void change_directory(struct FileSystem* fs, const char* path);
void execute_file(struct FileSystem* fs, const char* filename, int quiet);

#endif