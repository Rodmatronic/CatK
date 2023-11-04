void man(char* command)
{
    if (strcmp(command, "echo") == 0)
    {
        console_clear(COLOR_WHITE, COLOR_BLACK);
        printf("ECHO");
        console_gotoxy(VGA_WIDTH / 2 - 8, 0);
        printf("CatK user manual");
        console_gotoxy(VGA_WIDTH -6, 0);
        printf("ECHO");
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

    }
}