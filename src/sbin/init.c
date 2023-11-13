#include "config.h"

char* defaulthost = "catk";

void shell_process() {
    // Your shell process code here
    console_init(COLOR_WHITE, COLOR_BLACK);
    next_pid = 1;
    sh();
    // Once the shell exits, you may want to terminate the process.
}

int init(int debug)
{
    if (debug == 1)
    {
        return 0;
    }
    
    console_init(COLOR_WHITE, COLOR_BLACK);
    rows++;
    printf("Welcome to ");
    printf_brightcyan("CatK %s!\n", versionnumber);
    printf_dark("----------------------------------------\n");
    sleep(1);
    rows++;

    printf("Getting clock... ");
    GetCurrentTime();
    powerontimes();
    catkmessagefixed(1, 0);


    printf("\nCreating FS...");

    // Initialize the file table
    for (size i = 0; i < MAX_FILES; ++i) {
        rootfs.file_table[i].filename[0] = '\0';  // Empty filename indicates an unused entry
    }

    write_to_file(&rootfs, "init.catk", "Filesystem started successfully");

    catkmessagefixed(1, 0);
    
    int dummy = 0;
    if (fillfs == 1) {
        for (int i = 0; i < 1024; i++) {
            write_to_file(&rootfs, dummy, "root");
            dummy++;
        }
    }

    printf("\nAllocating memory...");
    initialize_memory();

    void* allocated_memory = allocate_memory(10 * 1024 * 1024);

    if (allocated_memory == NULL) {
        catkmessagefixed(3, 0);
    }else
    catkmessagefixed(1, 0);

    printf("\nGetting CPU info...");

    int result = cpuid_info(0);
    if (result) {
        // The function returned something
        catkmessagefixed(1, 0);
    } else {
        // The function did not return anything, indicating an error
        catkmessagefixed(3, 0);
    }

    printf("\nSetting hostname...");

    write_to_file(&rootfs, "hostname", defaulthostname);
    read_from_file(&rootfs, "hostname", buffer, sizeof(buffer));

    strcpy(host_name, buffer);

    // Check if host_name has been modified
    if (strlen(host_name) > 0) {
        catkmessagefixed(1, 1);
    } else {
        catkmessagefixed(3, 1);
    }

    printf("\nDetecting drives....");
    listdrivebits();
    rows++;
    rows++;
    printf("\nDetected drives!");
    catkmessagefixed(1, 2);

    printf("\nCreating rootuser...");
    // root user
    struct User rootUser;
    strcpy(rootUser.username, "root");
    strcpy(rootUser.shell, "/bin/sh");
    strcpy(username, rootUser.username);

    // Check if user has been modified
    if (strlen(username) > 0) {
        catkmessagefixed(1, 3);
    } else {
        catkmessagefixed(3, 3);
    }

    printf("\nSetting random seed...");
    int errorinseed = 0;
    seed = errorinseed;
    seed+=seconds;
    // Check if seed has been modified
    if (strlen(errorinseed) > 0) {
        catkmessagefixed(1, 4);
    } else {
        catkmessagefixed(3, 4);
    }

    printf("\nStarting ethernet...");

    // Assuming NAT configuration
    rows++;
    const char* statusNAT = isEthernetPluggedIn();
    
    const char* statusBridged = isEthernetPluggedIn();

    init_keyboard();
    if (bootargs != "quiet")
    {
        printf_dark("\nEnter full pathname for shell or RETURN for /bin/sh: \n");
        read(0);
    }

    console_init(COLOR_WHITE, COLOR_BLACK);
    // Create a shell process
    int shell_pid = fork(shell_process);

    if (shell_pid > 0) {
        // Execute the shell process
        execute_process(shell_pid);
    }
    
}