#pragma once

#define MAX_ITEMS_FSNODES 100

struct Nodes {
    char name[50];
    char* fs[50];
    char* device[50];
};

extern int nodecount;
extern char nodename[50];
extern struct Nodes fsnodes[MAX_ITEMS_FSNODES];

void makefsnode(struct Nodes list[], int *count, const char *nodename, char* fstype, char* device);
void displaynodes(const struct Nodes list[], int nodecount);
void setroot(const struct Nodes list[], int count, const char *mountPoint);