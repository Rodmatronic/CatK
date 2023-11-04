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

        read();

    }
}