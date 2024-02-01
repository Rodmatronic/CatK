#include <string.h>
#include <printk.h>
#include <fsnode.h>

struct Nodes fsnodes[MAX_ITEMS];

int nodecount = 0;
char nodename[50];

// Function to set a specified mount point as the root
void setroot(const struct Nodes list[], int count, const char *mountPoint) {
    int found = 0;
    for (int i = 0; i < count; ++i) {
        if (strcmp(list[i].name, mountPoint) == 0) {
            found = 1;
            break;
        }
    }

    if (!found) {
        printk("Error: Mount point '%s' not found.\n", mountPoint);
    }
}

// Add a filesystem node to the list of known nodes
void makefsnode(struct Nodes list[], int *count, const char *nodename, char* fstype) {
    if (*count < MAX_ITEMS) {
        strcpy(list[*count].name, nodename);
        strcpy(list[*count].fs, fstype);
        (*count)++;
        printk("created FS node '%s', with filesystem '%s'\n", nodename, fstype);
    } else {
        printk("node: cannot make node, too many nodes\n");
    }
}

void displaynodes(const struct Nodes list[], int count) {
    printk("all FS nodes:\n");
    for (int i = 0; i < count; ++i) {
        printk("%d: %s '%s'\n", i + 1, list[i].fs, list[i].name);
    }
}