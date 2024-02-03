#include <string.h>
#include <printk.h>
#include <fsnode.h>

struct Nodes fsnodes[MAX_ITEMS_FSNODES];

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
void makefsnode(struct Nodes list[], int *count, const char *nodename, char* fstype, char* device) {
    if (*count < MAX_ITEMS_FSNODES) {
        strcpy(list[*count].name, nodename);
        strcpy(list[*count].fs, fstype);
        strcpy(list[*count].device, device);
        (*count)++;
        printk("created FS node '%s', with filesystem '%s'\n", nodename, fstype);
    } else {
        printk("node: cannot make node, too many nodes\n");
    }
}

void displaynodes(const struct Nodes list[], int count) {
    for (int i = 0; i < count; ++i) {
        printk("%d: fs node '%s' with type '%s' at '%s'\n", i + 1, list[i].name, list[i].fs, list[i].device);
    }
}