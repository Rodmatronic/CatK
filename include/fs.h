// fs.h

#ifndef FS_H
#define FS_H

#include <stdint.h>

// Define a simple structure for a RAM file system entry (file or directory)
struct FileEntry {
    const char *name;
    const uint8_t *data;
    uint32_t size;
};

int addFile(struct RAMFileSystem *fs, const char *name, const uint8_t *data, uint32_t size);

struct Folder root;

#endif