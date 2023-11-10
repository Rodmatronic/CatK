void man(char* command)
{
    if (strcmp(command, "echo") == 0)
    {
        console_clear(COLOR_WHITE, COLOR_BLACK);
        printf("ECHO(1)");
        console_gotoxy(VGA_WIDTH / 2 - 8, 0);
        printf("CatK user manual");
        console_gotoxy(VGA_WIDTH -9, 0);
        printf("ECHO(1)");
        console_gotoxy(0, 2);
        printf("NAME");
        console_gotoxy(6, 3);
        printf("echo - display a line of text");
        console_gotoxy(0, 5);
        printf("SYNOPSIS");
        console_gotoxy(6, 6);
        printf("echo LONG-OPTION");
        console_gotoxy(0, 8);
        printf("DESCRIPTION");
        console_gotoxy(6, 9);
        printf("Echo the STRING(s) to standard output.");
        console_gotoxy(0, 11);
        printf("AUTHOR");
        console_gotoxy(6, 12);
        printf("Written by Brian Fox and Chet Ramey.");
        console_gotoxy(6, 13);
        printf("Ported to CatK by Rodmatronic");
        console_gotoxy(0, 15);
        printf("REPORTING BUGS");
        console_gotoxy(6, 16);
        printf("You can report issues at https://github.com/Rodmatronic/CatK/issues");
read();

    }else

    if (strcmp(command, "halt") == 0 || strcmp(command, "poweroff") == 0 || strcmp(command, "reboot") == 0)
    {
        console_clear(COLOR_WHITE, COLOR_BLACK);
        printf("HALT(8)");
        console_gotoxy(VGA_WIDTH / 2 - 8, 0);
        printf("CatK user manual");
        console_gotoxy(VGA_WIDTH -9, 0);
        printf("HALT(8)");
        console_gotoxy(0, 2);
        printf("NAME");
        console_gotoxy(6, 3);
        printf("halt, poweroff, reboot - Halt, power-off or reboot the machine");
        console_gotoxy(0, 5);
        printf("SYNOPSIS");
        console_gotoxy(6, 6);
        printf("halt, poweroff, reboot");
        console_gotoxy(0, 8);
        printf("DESCRIPTION");
        console_gotoxy(6, 9);
        printf("halt, poweroff, reboot may be used to halt, power-off, or reboot the");
        console_gotoxy(6, 10);
        printf("machine. All three commands take the same options.");
        console_gotoxy(0, 12);
        printf("REPORTING BUGS");
        console_gotoxy(6, 13);
        printf("You can report issues at https://github.com/Rodmatronic/CatK/issues");
read();
    }else

    if (strcmp(command, "uname") == 0)
    {
        console_clear(COLOR_WHITE, COLOR_BLACK);
        printf("UNAME(1)");
        console_gotoxy(VGA_WIDTH / 2 - 8, 0);
        printf("CatK user manual");
        console_gotoxy(VGA_WIDTH -10, 0);
        printf("UNAME(1)");
        console_gotoxy(0, 2);
        printf("NAME");
        console_gotoxy(6, 3);
        printf("uname - print system information");
        console_gotoxy(0, 5);
        printf("SYNOPSIS");
        console_gotoxy(6, 6);
        printf("uname [OPTION]...");
        console_gotoxy(0, 8);
        printf("DESCRIPTION");
        console_gotoxy(6, 9);
        printf("Print certain system information.  With no OPTION, same as -s.");
        console_gotoxy(6, 11);
        printf("-a, --all");
        console_gotoxy(12, 12);
        printf("print all information");
        console_gotoxy(6, 14);
        printf("-s, --kernel-name");
        console_gotoxy(12, 15);
        printf("print the kernel name");
        console_gotoxy(0, 17);
        printf("REPORTING BUGS");
        console_gotoxy(6, 18);
        printf("You can report issues at https://github.com/Rodmatronic/CatK/issues");
read();
    }else

    if (strcmp(command, "sh") == 0)
    {
        console_clear(COLOR_WHITE, COLOR_BLACK);
        printf("SH(1)");
        console_gotoxy(VGA_WIDTH / 2 - 8, 0);
        printf("CatK user manual");
        console_gotoxy(VGA_WIDTH -10, 0);
        printf("SH(1)");
        console_gotoxy(0, 2);
        printf("NAME");
        console_gotoxy(6, 3);
        printf("sh — command interpreter (shell)");
        console_gotoxy(0, 5);
        printf("SYNOPSIS");
        console_gotoxy(6, 6);
        printf("sh [-aCefnuvxIimqVEbp] [+aCefnuvxIimqVEbp] [-o option_name]");
        console_gotoxy(6, 7);
        printf("[+o option_name] [command_file [argument ...]]");
        console_gotoxy(0, 9);
        printf("DESCRIPTION");
        console_gotoxy(6, 10);
        printf("sh is the standard command interpreter for the system.");
        console_gotoxy(0, 12);
        printf("REPORTING BUGS");
        console_gotoxy(6, 13);
        printf("You can report issues at https://github.com/Rodmatronic/CatK/issues");
read();
    }else

    if (strcmp(command, "whoami") == 0)
    {
        console_clear(COLOR_WHITE, COLOR_BLACK);
        printf("WHOAMI(1)");
        console_gotoxy(VGA_WIDTH / 2 - 8, 0);
        printf("CatK user manual");
        console_gotoxy(VGA_WIDTH -10, 0);
        printf("WHOAMI(1)");
        console_gotoxy(0, 2);
        printf("NAME");
        console_gotoxy(6, 3);
        printf("whoami - print effective userid");
        console_gotoxy(0, 5);
        printf("SYNOPSIS");
        console_gotoxy(6, 6);
        printf("whoami [OPTION]...");
        console_gotoxy(0, 8);
        printf("DESCRIPTION");
        console_gotoxy(6, 9);
        printf("Print the user name associated with the current effective user ID. ");
        console_gotoxy(0, 11);
        printf("REPORTING BUGS");
        console_gotoxy(6, 12);
        printf("You can report issues at https://github.com/Rodmatronic/CatK/issues");
read();
    }else

    if (strcmp(command, "time") == 0)
    {
        console_clear(COLOR_WHITE, COLOR_BLACK);
        printf("TIME(1)");
        console_gotoxy(VGA_WIDTH / 2 - 7, 0);
        printf("CatK user manual");
        console_gotoxy(VGA_WIDTH -10, 0);
        printf("TIME(1)");
        console_gotoxy(0, 2);
        printf("NAME");
        console_gotoxy(6, 3);
        printf("time - Display the current system time.");
        console_gotoxy(0, 5);
        printf("SYNOPSIS");
        console_gotoxy(6, 6);
        printf("time [OPTION]...");
        console_gotoxy(0, 8);
        printf("DESCRIPTION");
        console_gotoxy(6, 9);
        printf("Print the current system time out to the terminal");
        console_gotoxy(0, 11);
        printf("REPORTING BUGS");
        console_gotoxy(6, 12);
        printf("You can report issues at https://github.com/Rodmatronic/CatK/issues");
read();
    }else
    if (strcmp(command, "hostname") == 0)
    {
        console_clear(COLOR_WHITE, COLOR_BLACK);
        printf("HOSTNAME(1)");
        console_gotoxy(VGA_WIDTH / 2 - 8, 0);
        printf("CatK user manual");
        console_gotoxy(VGA_WIDTH -12, 0);
        printf("HOSTNAME(1)");
        console_gotoxy(0, 2);
        printf("NAME");
        console_gotoxy(6, 3);
        printf("hostname - Set the system's host name");
        console_gotoxy(0, 5);
        printf("SYNOPSIS");
        console_gotoxy(6, 6);
        printf("time [OPTION]...");
        console_gotoxy(0, 8);
        printf("DESCRIPTION");
        console_gotoxy(6, 9);
        printf("hostname [hostname]");
        console_gotoxy(0, 11);
        printf("REPORTING BUGS");
        console_gotoxy(6, 12);
        printf("You can report issues at https://github.com/Rodmatronic/CatK/issues");
read();
    }else
    if (strcmp(command, "clear") == 0)
    {
        console_clear(COLOR_WHITE, COLOR_BLACK);
        printf("CLEAR(1)");
        console_gotoxy(VGA_WIDTH / 2 - 8, 0);
        printf("CatK user manual");
        console_gotoxy(VGA_WIDTH -8, 0);
        printf("CLEAR(1)");
        console_gotoxy(0, 2);
        printf("NAME");
        console_gotoxy(6, 3);
        printf("clear - clear the terminal screen");
        console_gotoxy(0, 5);
        printf("SYNOPSIS");
        console_gotoxy(6, 6);
        printf("clear [-Ttype]");
        console_gotoxy(0, 8);
        printf("DESCRIPTION");
        console_gotoxy(6, 9);
        printf("clear  clears your screen if this is possible, including its scrollback buffer");
        console_gotoxy(0, 12);
        printf("HISTORY");
        console_gotoxy(6, 13);
        printf("A clear command appeared in 2.79BSD dated February 24, 1979.");
        console_gotoxy(6, 14);
        printf("Later that was provided in Unix 8th edition (1985).");
        console_gotoxy(0, 16);
        printf("REPORTING BUGS");
        console_gotoxy(6, 17);
        printf("You can report issues at https://github.com/Rodmatronic/CatK/issues");
        read();
    }else
    printf("No such manual page");
}