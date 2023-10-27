#include "console.h"
#include "vga.h"
#include "runtimepid.h"

void echo(char* echotext) {
    Application echo;
    InitApplication(&echo, 1, "echo", MyAppClose);

    // Run your application
    //printf("Running %s with PID %d...\n", echo.AppName, echo.Pid);

    // When your application is done, close it
    printf("\n%s", echotext);
    AppClose(&echo);
}
