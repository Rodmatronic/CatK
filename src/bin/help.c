void help()
{
    Application help;
    InitApplication(&help, 1, "help", MyAppClose);
    printf("\n  CatK SH v0.01, index of pre-installed commands\n");
    printf_dark("    These shell commands are defined internally.  Type `help' to see this list.\n");

    printf("\n  echo");
    printf_dark(" - Prints the input provided by the user to the terminal\n");
    printf("  reboot");
    printf_dark(" - Turns the computer off then back on\n");
    printf("  time");
    printf_dark(" - Prints the current time in the terminal\n");
    printf("  uname");
    printf_dark(" - Prints the kernel version string to the terminal\n");
    printf("  hostname");
    printf_dark(" - Sets the kernel's hostname\n");
    printf("  clear");
    printf_dark(" - Clears the current terminal\n");
    printf("  read");
    printf_dark(" - Lets the user type input into the terminal\n");
    printf("  ls");
    printf_dark(" - Lists the current files and directories in the current folder\n");
    printf("  color");
    printf_dark(" - Temporarily changes the terminal colors\n");
    printf("  uname");
    printf_dark(" - Print system information\n");
    printf("  cpuid");
    printf_dark(" - CPUID provides an interface for querying information about the CPU.\n");
    printf("  whoami");
    printf_dark(" - Prints the current userid\n");
    printf("  sh");
    printf_dark(" - Command interpreter (shell)\n");
    printf("  times");
    printf_dark(" - Times stores important time-related info, and prints it to the screen\n");
    printf("  panic");
    printf_dark(" - WARNING!!! This is for debugging purposes, halts the system\n");
    AppClose(&help);
}