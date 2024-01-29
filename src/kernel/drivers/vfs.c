/*
//
//
//
//
//      NOTE! This is the SKELETON for the VFS, it does not work yet!!
//
//
//
//
//
//
*/

#include "printk.h"
#include "vfs.h"
#include "printk.h"
#include "fsnode.h"
#include "config.h"

// This would normally ask MOUNT (once that is made) what FS the files are on, and use that fs's driver to R/W/RM the file/directory.
// This is not implemented yet though

int v_createfile(char* filepath)
{
    printk("Could not create %s: not implemented\n", filepath);
    return 0;
}

int v_createdir(char* filepath)
{
    printk("Could not create %s: not implemented\n", filepath);
    return 0;
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
    printk("VFS init -------\n");
    makefsnode(fsnodes, &nodecount, sys_mountpoint, "RAMfs");
    makefsnode(fsnodes, &nodecount, "/dev", "devfs");
    makefsnode(fsnodes, &nodecount, "/proc", "procfs");

    printk("----------------------\n");

    displaynodes(fsnodes, nodecount);
}