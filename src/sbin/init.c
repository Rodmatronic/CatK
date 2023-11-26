#include "config.h"
#include "packages.c"
#include "startup.c"

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
    current_directory = "/proc";
    add_data_to_file(&rootfs, "kernel.logs", "init: Welcome to CatKernel");
    if (debug == 1)
    {
        return 0;
    }
    
    console_init(COLOR_WHITE, COLOR_BLACK);
    rows++;
    printf("Welcome to ");
    printf_brightcyan("CatK %s!\n", versionnumber);
    printf_dark("----------------------------------------\n");
    rows++;

    printf("Getting clock... ");
    GetCurrentTime();
    powerontimes();
    catkmessagefixed(1, 0);


    printf("\nCreating FS...");

    create_folder(&rootfs, "/bin", "/");
    create_folder(&rootfs, "/dev", "/");
    create_folder(&rootfs, "/etc", "/");
    create_folder(&rootfs, "/proc", "/");
    create_folder(&rootfs, "/usr", "/");
    create_folder(&rootfs, "/sys", "/");
    create_folder(&rootfs, "/run", "/");
    create_folder(&rootfs, "/mount", "/");

    current_directory = "/etc";
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

    /*printf("\nAllocating memory...");
    initialize_memory();

    void* allocated_memory = allocate_memory(10 * 1024 * 1024);

    if (allocated_memory == NULL) {
        add_data_to_file(&rootfs, "logs.d", "init: [ERR!] Could not allocate memory!\n");
        catkmessagefixed(3, 0);
    }else
    add_data_to_file(&rootfs, "logs.d", "init: [ ok ] Successfully Allocated memory!\n");
    catkmessagefixed(1, 0);*/

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
    read_from_file(&rootfs, "hostname", buffer, sizeof(buffer), 1);

    strcpy(host_name, buffer);

    // Check if host_name has been modified
    if (strlen(host_name) > 0) {
        catkmessagefixed(1, 1);
    } else {
        catkmessagefixed(3, 1);
    }

    printf("\nCreating tempfs...");
    create_folder(&rootfs, "/tmp", "/");

    printf("\nDetecting drives....");
    listdrivebits();
    rows++;
    rows++;
    current_directory = "/etc";
    write_to_file(&rootfs, "rootdisk", "root:disk0");
    printf("\nDetected drives!");
    catkmessagefixed(1, 2);

    printf("\nCreating binaries at /bin /sbin");
    rows++;

    //---- System binaries ----

    current_directory = "/sbin";
    write_to_file(&rootfs, "reboot", "type:App\nreboot");

    current_directory = "/sbin";
    write_to_file(&rootfs, "shutdown", "type:App\nshutdown");

    current_directory = "/sbin";
    write_to_file(&rootfs, "time", "type:App\ntime");

    current_directory = "/sbin";
    write_to_file(&rootfs, "halt", "type:App\nhalt");

    //---- Normal/userspace binaries ----

    current_directory = "/bin";
    write_to_file(&rootfs, "sh", "type:App\nsh");

    current_directory = "/bin";
    write_to_file(&rootfs, "login", "type:App\nlogin");

    current_directory = "/bin";
    write_to_file(&rootfs, "ls", "type:App\nls");

    current_directory = "/bin";
    write_to_file(&rootfs, "rm", "type:App\nrm");

    current_directory = "/bin";
    write_to_file(&rootfs, "uname", "type:App\nversion");

    current_directory = "/bin";
    write_to_file(&rootfs, "read", "type:App\nread");

    current_directory = "/bin";
    write_to_file(&rootfs, "read", "type:App\nread");

    current_directory = "/bin";
    write_to_file(&rootfs, "clear", "type:App\nclear");

    current_directory = "/bin";
    write_to_file(&rootfs, "game", "type:App\nclear\ncatascii-happy\nprint -----------------------------------------------\nprint Well hello, this is a simple game.\nprint -----------------------------------------------\nprint Press [ENTER]\nread\nclear\nprint COMMENCING SLEEP..\ncatascii-lookup\nprint -----------------------------------------------\nprint ?\nprint -----------------------------------------------\nprint Press [ENTER]\nread\ndelay\nclear\\ncatascii-tired\nprint_dark -----------------------------------------------\nprint_dark ...\nprint_dark -----------------------------------------------\ndelay\nclear\ncatascii-sleep\ndelay");

    current_directory = "/sbin";
    write_to_file(&rootfs, "startup", "");
    startup();

    printf("\nCreating rootuser...");
    // root user
    strcpy(rootUser.username, "root");
    strcpy(rootUser.shell, "/bin/sh");
    strcpy(username, rootUser.username);
    write_to_file(&rootfs, "session.catk", rootUser.username);
    create_folder(&rootfs, "/home", "/");
    create_folder(&rootfs, "/root", "/home");
    current_directory = "/home";
    write_to_file(&rootfs, "history.ksh", "");
    current_directory = "/etc";

    read_from_file(&rootfs, "session.catk", usersh_buffer, sizeof(usersh_buffer), 1);
    usersh = usersh_buffer;
    read_from_file(&rootfs, "hostname", hostnamesh_buffer, sizeof(hostnamesh_buffer), 1);
    hostnamesh = hostnamesh_buffer;

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

    current_directory = "/dev";
    write_to_file(&rootfs, "tty", "1");
    write_to_file(&rootfs, "keyboardsh", "1");
    write_to_file(&rootfs, "null", "0");
    write_to_file(&rootfs, "zero", "0");
    write_to_file(&rootfs, "random", seed);
    write_to_file(&rootfs, "fs", "rootfs");
    write_to_file(&rootfs, "mem", "1025");
    write_to_file(&rootfs, "sysmouse", "");
    current_directory = "/etc";

    printf("\nSetting up /proc ...");
    rows++;

    current_directory = "/proc";
    write_to_file(&rootfs, "args", bootargs);
    write_to_file(&rootfs, "arch", arch);
    current_directory = "/etc";

    printf("\nStarting Ethernet...");
    add_data_to_file(&rootfs, "logs.d", "init: [ .. ] Starting Ethernet...\n");

    // Assuming NAT configuration
    rows++;
    const char* statusNAT = isEthernetPluggedIn();
    
    const char* statusBridged = isEthernetPluggedIn();

    current_directory = "/dev";
    write_to_file(&rootfs, "net", "eth");
    current_directory = "/etc";


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
    const char* appContent = "type:App\nclear\nprint Test for Print\ntime\nHey, this is working!";
    current_directory = "/bin";
    write_to_file(&rootfs, "test", appContent);

    current_directory = "/etc";
    write_to_file(&rootfs, "packagelist", "");
    startuppkg();

    console_init(COLOR_WHITE, COLOR_BLACK);
    // Create a shell process
    int shell_pid = fork(shell_process);
    
    add_data_to_file(&rootfs, "logs.d", "init: [ .. ] Starting SH\n");
    
        // Execute the shell process
        current_directory = "/bin";
        execute_file(&rootfs, "sh");
    
}
