#include "config.h"

char* defaulthost = "catk";
char* motd = "Welcome to Catk! Type 'help', or just mess with the command line.\nYou can thange this message by editing /etc/motd";

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

    create_folder(&rootfs, "bin", "/");
    create_folder(&rootfs, "sys", "/");
    create_folder(&rootfs, "proc", "/");
    create_folder(&rootfs, "etc", "/");
    create_folder(&rootfs, "kernel", "/");
    create_folder(&rootfs, "dev", "/");

    current_directory = "etc";
    write_to_file(&rootfs, "logs.d", "init: Logfile created!\n");
    add_data_to_file(&rootfs, "logs.d", "init: [ ok ] Filesystem started successfully\n");

    write_to_file(&rootfs, "motd", motd);

    write_to_file(&rootfs, "version", "");

    add_data_to_file(&rootfs, "version", vername);
    add_data_to_file(&rootfs, "version", " ");
    add_data_to_file(&rootfs, "version", versionnumber);
    add_data_to_file(&rootfs, "version", " ");
    add_data_to_file(&rootfs, "version", prebootversion);

    catkmessagefixed(1, 0);
    
    int dummy = 0;
    if (fillfs == 1) {
        for (int i = 0; i < 1000; i++) {
            write_to_file(&rootfs, dummy, "root");
            dummy++;
        }
    }

    printf("\nAllocating memory...");
    initialize_memory();

    void* allocated_memory = allocate_memory(10 * 1024 * 1024);

    if (allocated_memory == NULL) {
        add_data_to_file(&rootfs, "logs.d", "init: [ERR!] Could not allocate memory!\n");
        catkmessagefixed(3, 0);
    }else
    add_data_to_file(&rootfs, "logs.d", "init: [ ok ] Successfully Allocated memory!\n");
    catkmessagefixed(1, 0);

    printf("\nGetting CPU info...");

    int result = cpuid_info(0);
    if (result) {
        // The function returned something
        add_data_to_file(&rootfs, "logs.d", "init: [ ok ] Added CPU file\n");
        catkmessagefixed(1, 0);
    } else {
        // The function did not return anything, indicating an error
        add_data_to_file(&rootfs, "logs.d", "init: [ERR!] Couldn't get CPU info!\n");
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

    printf("\nCreating tempfs...");
    create_folder(&rootfs, "tmp", "/");

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
    write_to_file(&rootfs, "session.catk", rootUser.username);

    // Check if user has been modified
    if (strlen(username) > 0) {
        add_data_to_file(&rootfs, "logs.d", "init: [ ok ] Added root user\n");
        catkmessagefixed(1, 3);
    } else {
        add_data_to_file(&rootfs, "logs.d", "init: [ERR!] Could not add root user!\n");
        catkmessagefixed(3, 3);
    }

    printf("\nSetting random seed...");
    int errorinseed = 0;
    seed = errorinseed;
    seed+=seconds;

    // Check if seed has been modified
    if (strlen(errorinseed) > 0) {
        add_data_to_file(&rootfs, "logs.d", "init: [ ok ] Set random seed\n");
        catkmessagefixed(1, 4);
    } else {
        add_data_to_file(&rootfs, "logs.d", "init: [ERR!] Could not set random seed\n");
        catkmessagefixed(3, 4);
    }

    printf("\nMaking dev nodes...");

    current_directory = "dev";
    write_to_file(&rootfs, "tty", "1");
    write_to_file(&rootfs, "null", NULL);
    write_to_file(&rootfs, "zero", "0");
    write_to_file(&rootfs, "random", seed);
    write_to_file(&rootfs, "fs", "rootfs");
    write_to_file(&rootfs, "mem", "1025");
    current_directory = "etc";

    current_directory = "proc";
    write_to_file(&rootfs, "args", bootargs);
    current_directory = "etc";

    printf("\nStarting Ethernet...");
    add_data_to_file(&rootfs, "logs.d", "init: [ .. ] Starting Ethernet...\n");

    // Assuming NAT configuration
    rows++;
    const char* statusNAT = isEthernetPluggedIn();
    
    const char* statusBridged = isEthernetPluggedIn();

    current_directory = "dev";
    write_to_file(&rootfs, "net", "eth");
    current_directory = "etc";


    init_keyboard();
    if (bootargs != "quiet")
    {
        add_data_to_file(&rootfs, "logs.d", "init: [ .. ] Not quiet, showing popup\n");
        printf_dark("\nEnter full pathname for shell or RETURN for /bin/sh: \n");
        read(0);
    }

    if (bootargs == "quiet")
    {
        add_data_to_file(&rootfs, "logs.d", "init: [ .. ] Quiet, hiding popup...\n");
    }

    add_data_to_file(&rootfs, "logs.d", "init: [ .. ] Creating test application\n");
    
    // Create a file with the first line "type:App"
    const char* appContent = "type:App\nMore test app";

    current_directory = "bin";
    write_to_file(&rootfs, "test.app", appContent);

    console_init(COLOR_WHITE, COLOR_BLACK);
    // Create a shell process
    int shell_pid = fork(shell_process);
    
    add_data_to_file(&rootfs, "logs.d", "init: [ .. ] Starting SH\n");

    if (shell_pid > 0) {
        // Execute the shell process
        current_directory = "/";
        execute_process(shell_pid);
    }
    
}
