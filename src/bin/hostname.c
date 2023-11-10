#include "config.h"

void hostname(char* input) {
    if (strlen(input) < sizeof(host_name)) {
        strcpy(host_name, input);
    } else {
        printf("Hostname is too long. Maximum length is %d characters.\n", sizeof(host_name) - 1);
    }
}