#include "printk.h"
#include "vfs.h"
#include "printk.h"
#include "fsnode.h"
#include "config.h"
#include "string.h"
#include "ramfs.h"

// This would normally ask MOUNT (once that is made) what FS the files are on, and use that fs's driver to R/W/RM the file/directory.
// This is not implemented yet though

int v_createfile(char* filepath)
{
    printk("Could not create %s: not implemented\n", filepath);
    return 0;
}

int v_createdir(char* filepath) {
    // Extract the filesystem type from the root node
    char rootFS[50];
    strcpy(rootFS, fsnodes[0].fs);

    if (strcmp(rootFS, "RAMfs") == 0) {
        printk("Creating directory '%s' in RAMfs.\n", filepath);
        ramfs_createdir(filepath);
    } else {
        printk("Could not create %s: not implemented for filesystem '%s'\n", filepath, rootFS);
        return 0;
    }

    // Directory creation successful
    return 1;
}

int v_readfile(char* filepath)
{
    printk("Could not read %s: I/O error\n", filepath);
    return 0;
}

int v_rmfile(char* filepath)
{
    printk("Could not remove %s: I/O error\n", filepath);
    return 0;
}

int v_sync()
{
    printk("Failed to sync VFS: Unknown error");
    return 0;
}

void vfs_init()
{

}