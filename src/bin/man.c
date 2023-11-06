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

    }

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
    }

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
    }

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
    }

    read();
}