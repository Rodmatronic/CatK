#include "fs.h"
#include "config.h"
#include "libc.h"

void pkg(char* package)
{
    printf("Fetching package %s...\n", package);
    current_directory = "/etc";
    add_data_to_file(&rootfs, "packagelist", "root");
    
}