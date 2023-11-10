#include "config.h"

char* defaulthost = "catk";

void shell_process() {
    // Your shell process code here
    console_init(COLOR_WHITE, COLOR_BLACK);
    next_pid = 1;
    sh();
    // Once the shell exits, you may want to terminate the process.
}

void init()
{
    console_init(COLOR_WHITE, COLOR_BLACK);

    bootmessage("Getting CPU info...");
    int result = cpuid_info(0);

    if (result) {
        // The function returned something
        printf("%s\n", result);
        printf("Got CPU info!");
        catkmessage(1);
    } else {
        // The function did not return anything, indicating an error
        printf("cpuid_info did not return anything.\n");
        catkmessage(3);
    }

    printf("Setting hostname...");
    strcpy(host_name, defaulthost);

    // Check if host_name has been modified
    if (strlen(host_name) > 0) {
        catkmessage(1);
    } else {
        catkmessage(3);
    }

    printf("Detecting drives....\n");
    listdrivebits();
    printf("Detected drives!");
    catkmessage(1);

    printf("Creating rootuser...");
    // root user
    struct User rootUser;
    strcpy(rootUser.username, "root");
    strcpy(rootUser.shell, "/bin/sh");
    strcpy(username, rootUser.username);

    // Check if user has been modified
    if (strlen(username) > 0) {
        catkmessage(1);
    } else {
        catkmessage(3);
    }

    printf("Setting random seed...");
    int errorinseed = 0;
    seed = errorinseed;
    seed+=seconds;
    // Check if seed has been modified
    if (strlen(errorinseed) > 0) {
        catkmessage(1);
    } else {
        catkmessage(3);
    }

    printf("Starting ethernet...\n");

    // Assuming NAT configuration
    const char* statusNAT = isEthernetPluggedIn();

    const char* statusBridged = isEthernetPluggedIn();

    printf("Starting sh process...\n");

    init_keyboard();
    if (bootargs != "quiet")
    {
        printf_dark("Enter full pathname for shell or RETURN for /bin/sh: \n");
        read();
    }

    console_init(COLOR_WHITE, COLOR_BLACK);
    // Create a shell process
    int shell_pid = fork(shell_process);

    if (shell_pid > 0) {
        // Execute the shell process
        execute_process(shell_pid);
    }
    
}