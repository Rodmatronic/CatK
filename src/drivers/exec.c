#include "fs.h"
#include "sh.h"
#include "console.h"
#include "time.h"
#include "read.h"
#include "config.h"
#include "libc.h"
#include "string.h"
#include "panic.h"
#include "syspw.h"
#include "vga.h"
#include "bitmap.h"
#include "kernel.h"
#include "interface.h"
#include "process.h"
#include "desktop.h"

void execute_file(struct FileSystem* fs, const char* filename) {
    char buffer[BLOCK_SIZE];
    read_from_file(fs, filename, buffer, BLOCK_SIZE, 0);
    write_serial("exec: starting: ");
    write_serial(filename);
    write_serial("\n");

    // Check if the file is of type "app"
    char* typeToken = k_strstr(buffer, "type:App");
    if (typeToken == NULL) {
        return;
    }

    addProcess(filename);
    rows+=25;

    // Print the values for demonstration
    //printf("Process ID: %d\n", pid);
    //printf("Process Name: %s\n", process.name);
    write_serial("\n");
    write_serial("Started process! Process name: ");
    write_serial(processes[pid].name);
    write_serial("\n");

    // Tokenize and execute each line
    char* token = k_strtok(buffer, "\n");
    while (token != NULL) {
        vga_disable_cursor();
        
        char* Reboot = k_strstr(token, "reboot");
        if (Reboot != NULL) {
            syspw(0);
        }
        char* shutdown = k_strstr(token, "shutdown");
        if (shutdown != NULL) {
            syspw(1);
        }
        char* graphics = k_strstr(token, "graphics_init");
        if (graphics != NULL) {
            desktop();
        }
        // Check if the line contains "print "
        char* printToken = k_strstr(token, "print ");
        if (printToken != NULL) {
            // Print the text following "print "
            printf("%s\n", printToken + strlen("print "));
            break;;
        } else {
        char* printTokendark = k_strstr(token, "print_dark ");
        if (printTokendark != NULL) {
            // Print the text following "print "
            printf("%s\n", printTokendark + strlen("print_dark "));
        }
        char* delaycommand = k_strstr(token, "delay");
        if (delaycommand != NULL) {
            //sleep(1);
        }
        char* shcommand = k_strstr(token, "sh");
        if (shcommand != NULL) {
            current_directory = "/";
            k_sh();
        }
        char* Time = k_strstr(token, "time");
        if (Time != NULL) {
            GetCurrentTime();
        }
        char* clearToken = k_strstr(token, "clear");
        if (clearToken != NULL) {
            // Print the text following "clear"
            rows = 0;
            rows--;
            console_init(COLOR_WHITE, COLOR_BLACK);
        }/*
        char* initcommand = k_strstr(token, "init");
        if (initcommand != NULL) {
            init();
        }
        char* logincommand = k_strstr(token, "login");
        if (logincommand != NULL) {
            login();
        }
        char* version = k_strstr(token, "version");
        if (version != NULL) {
            read_from_file(&rootfs, "version", buffer, sizeof(buffer), 1);
            printf("%s", buffer);
        }*/
        char* readcommand = k_strstr(token, "read");
        if (readcommand != NULL) {
            read(0);
        }
        char* paniccommand = k_strstr(token, "panic");
        if (paniccommand != NULL) {
            panic("Triggered by Executable");
        }
        char* haltcommand = k_strstr(token, "halt");
        if (haltcommand != NULL) {
            syspw(2);
        }
        char* catasciilookupcommand = k_strstr(token, "catascii-lookup");
        if (catasciilookupcommand != NULL) {
            printf("                   _      _\n");
            printf("                  / \\    / \\\n");
            printf("                 /   \\__/   \\\n");
            printf("                /   |    |   \\\n");
            printf("               |      -       |\n");
            printf("              =|       o      |=\n");
            printf("              =\\              /=\n");
            printf("                \\            /\n");
            printf("                 =====\\/=====\n");
            printf("                    (CatK)\n");
        }
        char* catasciihappycommand = k_strstr(token, "catascii-happy");
        if (catasciihappycommand != NULL) {
            printf("                   _      _\n");
            printf("                  / \\    / \\\n");
            printf("                 /   \\__/   \\\n");
            printf("                /            \\\n");
            printf("               |    |    |    |\n");
            printf("              =|      -       |=\n");
            printf("              =\\      v       /=\n");
            printf("                \\            /\n");
            printf("                 =====\\/=====\n");
            printf("                    (CatK)\n");
        }
        char* catasciithinkcommand = k_strstr(token, "catascii-eh");
        if (catasciithinkcommand != NULL) {
            printf("                   _      _\n");
            printf("                  / \\    / \\\n");
            printf("                 /   \\__/   \\\n");
            printf("                /            \\\n");
            printf("               |     |    |   |\n");
            printf("              =|      -       |=\n");
            printf("              =\\      _       /=\n");
            printf("                \\            /\n");
            printf("                 =====\\/=====\n");
            printf("                    (CatK)\n");
        }
        char* catasciisleepcommand = k_strstr(token, "catascii-sleep");
        if (catasciisleepcommand != NULL) {
            printf("                   _      _\n");
            printf("                  / \\    / \\         z\n");
            printf("                 /   \\__/   \\     z\n");
            printf("                /            \\   z\n");
            printf("               |    _    _    |\n");
            printf("              =|      -       |=\n");
            printf("              =\\      D       /=\n");
            printf("                \\            /\n");
            printf("                 =====\\/=====\n");
            printf("                    (CatK)\n");
        }
        char* catasciisleepycommand = k_strstr(token, "catascii-tired");
        if (catasciisleepycommand != NULL) {
            printf("                   _      _\n");
            printf("                  / \\    / \\         z\n");
            printf("                 /   \\__/   \\     z\n");
            printf("                /            \\   z\n");
            printf("               |    _    _    |\n");
            printf("              =|      -       |=\n");
            printf("              =\\      w       /=\n");
            printf("                \\            /\n");
            printf("                 =====\\/=====\n");
            printf("                    (CatK)\n");
        }
        // Move to the next line
        vga_enable_cursor();
        token = k_strtok(NULL, "\n");
        }
    }
}