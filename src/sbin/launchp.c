/*
 * Start any services that are dictated here and exist in /sbin
 *
 */
#include "kernel.h"
#include "libc.h"
#include "fs.h"
#include "exec.h"
#include "types.h"
#include "string.h"
#include "config.h"
#include "console.h"
#include "timer.h"

int launchp(TIMER_FUNC_ARGS *args)
{
    char* workingdir = current_directory;
    char buffer[BLOCK_SIZE];
    current_directory = "/etc";
    read_from_file(&rootfs, "launchp", buffer, BLOCK_SIZE, 1);
    current_directory = "/sbin";
    // Tokenize and execute each line
    char* token = k_strtok(buffer, "\n");
    while (token != NULL) {
        execute_file(&rootfs, token);
        kernmessage(token);

        token = k_strtok(NULL, "\n");
    }
    current_directory = workingdir;
    return 0;
}