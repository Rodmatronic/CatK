#include "fs.h"
#include "config.h"

void startup()
{
    current_directory = "/mount";
    write_to_file(&rootfs, "liveroot", "/");
    current_directory = "/etc";
}