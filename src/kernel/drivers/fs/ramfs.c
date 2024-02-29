#include "fsnode.h"
#include "printk.h"
#include "config.h"
#include "vfs.h"
#include "ramfs.h"
#include <string.h>

struct RAMfs ramfs;

void ramfs_init(struct RAMfs *ramfs) {
    ramfs->entryCount = 0;
}

void createramfs()
{
    printk("RAMfs: init\n");
    makefsnode(fsnodes, &nodecount, sys_mountpoint, "RAMfs", "ramdisk1");
    setroot(fsnodes, nodecount, "/");
    v_createdir("/bin");
    v_createdir("/sbin");
    v_createdir("/dev");
    v_createdir("/etc");
    v_createdir("/lib");
}

void ramfs_createdir(struct RAMfs *ramfs, const char *path) {
    if (ramfs->entryCount < MAX_ITEMS) {
        // Check if the path already exists
        for (int i = 0; i < ramfs->entryCount; ++i) {
            if (strcmp(ramfs->entries[i].name, path + 1) == 0) {  // Skip the leading slash
                printk("RAMfs: directory '%s' already exists\n", path);
                return;
            }
        }

        struct RAMfsEntry *entry = &ramfs->entries[ramfs->entryCount];
        strcpy(entry->name, path);
        strcpy(entry->type, "directory");
        entry->content[0] = '\0';  // Empty content for directories
        ramfs->entryCount++;
    } else {
        printk("RAMfs: cannot create directory, too many entries\n");
    }
}

void ramfs_create_file(struct RAMfs *ramfs, const char *path, const char *content) {
    if (ramfs->entryCount < MAX_ITEMS) {
        // Check if the path already exists
        for (int i = 0; i < ramfs->entryCount; ++i) {
            if (strcmp(ramfs->entries[i].name, path + 1) == 0) {  // Skip the leading slash
                printk("RAMfs: file '%s' already exists\n", path);
                return;
            }
        }

        struct RAMfsEntry *entry = &ramfs->entries[ramfs->entryCount];
        strcpy(entry->name, path);
        strcpy(entry->type, "file");
        strcpy(entry->content, content);
        ramfs->entryCount++;
    } else {
        printk("RAMfs: cannot create file, too many entries\n");
    }
}


void ramfs_remove_entry(struct RAMfs *ramfs, const char *name) {
    for (int i = 0; i < ramfs->entryCount; ++i) {
        if (strcmp(ramfs->entries[i].name, name) == 0) {
            // Remove the entry by shifting elements
            for (int j = i; j < ramfs->entryCount - 1; ++j) {
                strcpy(ramfs->entries[j].name, ramfs->entries[j + 1].name);
                strcpy(ramfs->entries[j].type, ramfs->entries[j + 1].type);
                strcpy(ramfs->entries[j].content, ramfs->entries[j + 1].content);
            }
            ramfs->entryCount--;
            //printk("Removed RAMfs entry '%s'\n", name);
            return;
        }
    }
    printk("RAMfs: entry '%s' not found\n", name);
}

void ramfs_display_entries(const struct RAMfs *ramfs) {
    for (int i = 0; i < ramfs->entryCount; ++i) {
        printk("%s\n", ramfs->entries[i].name);
    }
}