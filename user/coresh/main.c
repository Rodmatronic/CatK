#include "unistd.h"
#include "string.h"
#include <stdint.h>

#define X_OK 1

#define MAX_INPUT 1024
#define DIR_LIST_SIZE 3

#define print(x) \
  write(1, x, strlen(x))

#define print_err(x) \
  write(2, x, strlen(x))

const char *directories[DIR_LIST_SIZE] = {
    "/bin",
    "/usr/bin",
    "/usr/local/bin"
};

void execute_command(const char *cmd, char *args[]) {
    char path[MAX_INPUT];
    pid_t pid;

    for (int i = 0; i < DIR_LIST_SIZE; i++) {
        int path_len = 0;
        const char *dir = directories[i];

        // Build the full path
        while (*dir) {
            path[path_len++] = *dir++;
        }
        path[path_len++] = '/';

        const char *c = cmd;
        while (*c) {
            path[path_len++] = *c++;
        }
        path[path_len] = '\0';

        if (access(path, X_OK) == 0) {
            pid = fork();
            if (pid == 0) {
                execv(path, args);
                _exit(1); // Exit if execv fails
            } else if (pid > 0) {
                wait(NULL);
                return;
            }
        }
    }
    print_err(cmd);
    print_err(": command not found\n");
}

int main() {
    print("\033[2J");
    print("\033[0;0f");
    print("\033[1;31mI am fizzbuzz..\033[1;0m\n");
    char input[MAX_INPUT];
    while (1) {
        print("coresh$ ");
        int len = read(0, input, MAX_INPUT);
        if (len <= 0) {
            continue;
        }
        if (input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        if (len > 1) { // Skip if input is empty
            char *args[MAX_INPUT / 2 + 1];
            int arg_count = 0;

            char *token = strtok(input, " ");
            while (token != NULL) {
                args[arg_count++] = token;
                token = strtok(NULL, " ");
            }
            args[arg_count] = NULL;

            // Check for built-in commands
            if (strcmp(args[0], "cd") == 0) {
                if (arg_count > 1) {
                    if (chdir(args[1]) != 0) {
                        print_err("cd: No such file or directory\n");
                    }
                } else {
                    print_err("cd: missing argument\n");
                }
            } else if(strcmp(args[0], "echo") == 0) {
                for(int i = 1; args[i]; i++) {
                  print(args[i]);
                  print(" ");
                }
                print("\n");
            } else {
                // Execute external commands
                execute_command(args[0], args);
            }
        }
        memset(input, 0, MAX_INPUT);
    }
    return 0;
}
