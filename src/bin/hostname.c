#include "config.h"

void hostname(char* input) {
    if (strlen(input) < sizeof(host_name)) {
        write_to_file(&rootfs, "hostname", input);
        read_from_file(&rootfs, "hostname", buffer, sizeof(buffer));

        strcpy(host_name, buffer);
        
    } else {
        printf("Hostname is too long. Maximum length is %d characters.\n", sizeof(host_name) - 1);
    }
}