#include "config.h"

void hostname(char* input) {
    if (strlen(input) < sizeof(host_name)) {

        char* working_dir = current_directory;
        rm_file(&rootfs, "/hostname");
        write_to_file(&rootfs, "/hostname", defaulthostname);
        read_from_file(&rootfs, "/hostname", buffer, sizeof(buffer), 1);

        strcpy(host_name, buffer);

        current_directory = current_directory;
        
    } else {
        printf("Hostname is too long. Maximum length is %d characters.\n", sizeof(host_name) - 1);
    }
}
