#include "types.h"
#include "console.h"
#include "libc.h"
#include "string.h"
#include "process.h"

uint16 pid = 0;  // Process ID

struct processid processes[MAX_ENTRIES];

// Function to add a process to the list
void addProcess(const char *name) {
    if (pid < MAX_ENTRIES) {
        strncpy(processes[pid].name, name, sizeof(processes[pid].name) - 1);
        processes[pid].name[sizeof(processes[pid].name) - 1] = '\0'; // Ensure null-termination
        pid++;
        //printf("Added process: %s\n", name);
    } else {
        printf("Max number of entries reached. Cannot add more processes.\n");
    }
}

// Function to list all processes
void listProcesses() {
    printf("List of Processes:\n");
    for (uint16 i = 0; i < pid; ++i) {
        printf("%d. %s\n", i + 1, processes[i].name);
    }
}