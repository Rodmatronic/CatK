#include "fs.h"
#include "libc.h"
#include "config.h"
#include "console.h"

void sysdiaginit()
{
    printf("sysdiag started\n");
    char* workingdirectory = current_directory;
    current_directory = "/etc";
    write_to_file(&rootfs, "logs", "sysdaig started from 'sysdiaginit'");
    current_directory = workingdirectory;
}