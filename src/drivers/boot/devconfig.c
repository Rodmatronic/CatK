#include "libc.h"
#include "keyboard.h"
#include "fs.h"
#include "console.h"
#include "config.h"

void BootDevConfig()
{
    char* working_directory = current_directory;
    current_directory = "/dev";
    write_to_file(&rootfs, "console", "1");
    printf("%C   Created /dev/console\n", 0x8, 0x0);
    
    write_to_file(&rootfs, "keyboard", "1");
    init_keyboard();
    printf("%C   Made devnode for keyboard\n", 0x8, 0x0);

    write_to_file(&rootfs, "serial", "1");
    init_keyboard();
    printf("%C   Made devnode for serial\n", 0x8, 0x0);

    write_to_file(&rootfs, "null", "0");
    printf("%C   Created /dev/null\n", 0x8, 0x0);

    write_to_file(&rootfs, "random", "1234");
    printf("%C   Created /dev/random\n", 0x8, 0x0);

    write_to_file(&rootfs, "tty0", "1");
    printf("%C   Created /dev/tty1\n", 0x8, 0x0);

    write_to_file(&rootfs, "tty1", "0");
    printf("%C   Created /dev/tty1\n", 0x8, 0x0);

    write_to_file(&rootfs, "tty2", "0");
    printf("%C   Created /dev/tty2\n", 0x8, 0x0);

    write_to_file(&rootfs, "tty3", "0");
    printf("%C   Created /dev/tty3\n", 0x8, 0x0);

    write_to_file(&rootfs, "tty4", "0");
    printf("%C   Created /dev/tty4\n", 0x8, 0x0);

    write_to_file(&rootfs, "tty5", "0");
    printf("%C   Created /dev/tty5\n", 0x8, 0x0);

    write_to_file(&rootfs, "tty6", "0");
    printf("%C   Created /dev/tty6\n", 0x8, 0x0);

    current_directory = working_directory;
    return;
}