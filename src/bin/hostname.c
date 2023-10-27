char host_name[30] = ""; // hostname with a maximum length of 63 characters
char username[] = "root"; // username

void hostname(char* input) {
    if (strlen(input) < sizeof(host_name)) {
        strcpy(host_name, input);
    } else {
        printf("Hostname is too long. Maximum length is %d characters.\n", sizeof(host_name) - 1);
    }
}