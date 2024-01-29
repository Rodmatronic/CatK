#pragma once

#define MAX_ITEMS 100

struct Nodes {
    char name[50];
};

extern int nodecount;
extern char nodename[50];
extern struct Nodes fsnodes[MAX_ITEMS];

void makefsnode(struct Nodes list[], int *count, const char *nodename);
void displaynodes(const struct Nodes list[], int nodecount);