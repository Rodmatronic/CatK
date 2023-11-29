#include "fs.h"

void sysdiaginit()
{
    char* workingdirectory = current_directory;
    current_directory = "/etc";
    write_to_file(&rootfs, "logs", "sysdaig started from 'sysdiaginit'");
    current_directory = workingdirectory;
}