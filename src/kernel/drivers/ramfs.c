#include "fsnode.h"
#include "printk.h"
#include "config.h"
#include "vfs.h"

void createramfs()
{
    printk("RAMfs init -------\n");
    makefsnode(fsnodes, &nodecount, sys_mountpoint, "RAMfs");
    setroot(fsnodes, &nodecount, "/");

    v_createdir("/bin");
    v_createdir("/sbin");
    v_createdir("/dev");
    v_createdir("/etc");
    v_createdir("/lib");

    printk("----------------------\n");
}

void ramfs_createdir(char* filepath)
{}