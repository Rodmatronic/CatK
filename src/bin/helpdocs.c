#include "fs.h"
#include "config.h"

void createhelpdocs()
{
    create_folder(&rootfs, "docs", "/etc");
    current_directory = "/etc/docs";
    write_to_file(&rootfs, "helpdocs1", "Catk help docs\n");
    add_data_to_file(&rootfs, "helpdocs1", "-------------------------------------\n");
    add_data_to_file(&rootfs, "helpdocs1", " cd - [dir]        echo - [input]\n");
    add_data_to_file(&rootfs, "helpdocs1", " ls - [input]      halt - n/a\n");
    add_data_to_file(&rootfs, "helpdocs1", " rm - [input]      time - n/a\n");
    add_data_to_file(&rootfs, "helpdocs1", " reboot - n/a      cat  - [input]\n");
    add_data_to_file(&rootfs, "helpdocs1", " shutdown - n/a    help - n/a\n");
    add_data_to_file(&rootfs, "helpdocs1", " halt - n/a        exec - [input]\n");
    add_data_to_file(&rootfs, "helpdocs1", " whoami - n/a      hostname - [input]\n");
    add_data_to_file(&rootfs, "helpdocs1", " dmesg - n/a       uname - n/a\n");
    add_data_to_file(&rootfs, "helpdocs1", " mkdir - [input]   history - n/a\n");
    add_data_to_file(&rootfs, "helpdocs1", " about - n/a       useradd - [input]\n");
    add_data_to_file(&rootfs, "helpdocs1", " panic - [input]\n");
    add_data_to_file(&rootfs, "helpdocs1", "-------------------------------------\n");
}
/*
        printf("\n%CCatK %s k_sh help\n\n", 0xB, 0x0, versionnumber);
        printf("cd - [dir]        echo - [input]\n");
        printf("ls - [input]      halt - n/a\n");
        printf("rm - [input]      time - n/a\n");
        printf("reboot - n/a      cat  - [input]\n");
        printf("shutdown - n/a    help - n/a\n");
        printf("halt - n/a        exec - [input]\n");
        printf("whoami - n/a      hostname - [input]\n");
        printf("dmesg - n/a       uname - n/a\n");
        printf("mkdir - [input]   history - n/a\n");
        printf("about - n/a       adduser - [input]\n");
        printf("panic - [input]\n");
        rows+=12;
*/