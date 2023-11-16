#include "config.h"

void hostname(char* input) {
    if (strlen(input) < sizeof(host_name)) {
        current_directory = "etc";
        rm_file(&rootfs, "hostname");
        write_to_file(&rootfs, "hostname", input);
        read_from_file(&rootfs, "hostname", buffer, sizeof(buffer));

        strcpy(host_name, buffer);

        current_directory = "/";
        
    } else {
        printf("Hostname is too long. Maximum length is %d characters.\n", sizeof(host_name) - 1);
    }
}
