#pragma once

#include <string.h>
#include <printk.h>

#define MAX_ITEMS 10000

struct RAMfsEntry {
    char name[50];
    char type[10];  // "file" or "directory"
    char content[9];  // Content for files, empty for directories
};

struct RAMfs {
    struct RAMfsEntry entries[MAX_ITEMS];
    int entryCount;
};

extern struct RAMfs ramfs;

void ramfs_init(struct RAMfs *ramfs);
void ramfs_createdir(struct RAMfs *ramfs, const char *name);
void ramfs_create_file(struct RAMfs *ramfs, const char *name, const char *content);
void ramfs_remove_entry(struct RAMfs *ramfs, const char *name);
void ramfs_display_entries(const struct RAMfs *ramfs);
void createramfs();