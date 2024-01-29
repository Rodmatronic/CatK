#include <string.h>
#include <printk.h>
#include <fsnode.h>

struct Nodes fsnodes[MAX_ITEMS];

int nodecount = 0;
char nodename[50];

void makefsnode(struct Nodes list[], int *count, const char *nodename) {
    if (*count < MAX_ITEMS) {
        strcpy(list[*count].name, nodename);
        (*count)++;
        printk("created FS node '%s'\n", nodename);
    } else {
        printk("node: cannot make node, too many nodes\n");
    }
}

void displaynodes(const struct Nodes list[], int count) {
    printk("all FS nodes:\n");
    for (int i = 0; i < count; ++i) {
        printk("%d. %s\n", i + 1, list[i].name);
    }
}