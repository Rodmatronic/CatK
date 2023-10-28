// Define a structure to represent a process
typedef struct {
    int id; // Unique process ID
    void (*entry)(); // Function pointer to the process entry point
    int state; // Process state (e.g., running, terminated)
} Process;

#define MAX_PROCESSES 999
Process processes[MAX_PROCESSES];
int next_pid = 1;

void top() {
    console_clear(COLOR_BLACK, COLOR_WHITE);
    console_gotoxy(0, 0);
    printf("PID   STATE\n");
    printf("------------\n");

    for (int i = 0; i < next_pid; i++) {
        int pid = processes[i].id;
        int state = processes[i].state;

        if (pid != -1) {
            console_gotoxy(0, i + 2);
            printf("%d     %d\n", pid, state);
        }
    }

    while (1) {
        // Simple process viewer
        // Refresh the process list
        console_gotoxy(0, 2);
        for (int i = 0; i < next_pid; i++) {
            int pid = processes[i].id;
            int state = processes[i].state;

            if (pid != -1) {
                printf("   \b\b\b\b\b\b"); // Clear the previous PID and state
                console_gotoxy(0, i + 2);
                printf("%d     %d\n", pid, state);
            }
        }
    }
}