#pragma once

#define MAX_ITEMS 100

struct Nodes {
    char name[50];
    char* fs[256];
};

extern int nodecount;
extern char nodename[50];
extern struct Nodes fsnodes[MAX_ITEMS];

void makefsnode(struct Nodes list[], int *count, const char *nodename, char* fstype);
void displaynodes(const struct Nodes list[], int nodecount);
void setroot(const struct Nodes list[], int count, const char *mountPoint);