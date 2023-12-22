#ifndef PROCESS_H
#define PROCESS_H

#include "types.h"

#define MAX_ENTRIES 999

extern uint16 pid;  // Process ID

struct processid {
    char name[256];  // Process name (maximum length of 255 characters)
};

extern struct processid processes[MAX_ENTRIES];


void addProcess(const char *name);

// Function to list all processes
void listProcesses();


#endif