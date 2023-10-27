void PreBoot()
{
    console_gotoxy(0, 0);
    printf(" _______   _____   _______  ___   _ \n");
    printf("|   ____| /  _  \\ |       ||   | | |\n");
    printf("|  |     |  | |  ||_     _||   |_| |\n");
    printf("|  |     |  |_|  |  |   |  |      _|\n");
    printf("|  |     |       |  |   |  |     |_ \n");
    printf("|  |____ |   _   |  |   |  |    _  |\n");
    printf("|_______||__| |__|  |___|  |___| |_|\n\n");

    printf("+==================================+\n");
    printf("|    CatK PreBoot\n");
    printf("| Boot normally [Enter]\n");
    printf("| 1.  Supress boot logs\n");
    printf("| 2.  Reboot\n");
    printf("|\n");
    printf("|\n");
    printf("|\n");
    printf("|\n");
    printf("|\n");
    printf("+==================================+\n");

    console_gotoxy(0, 24);
    printf_dark("Build num 0.03");

    console_gotoxy(45, 10);
    printf_blue("       _      _");
        console_gotoxy(45, 9);
    printf_blue("      / \\    / \\");
        console_gotoxy(45, 10);
    printf_blue("     /   \\__/   \\");
        console_gotoxy(45, 11);
    printf_blue("    /            \\");
        console_gotoxy(45, 12);
    printf_blue("   |    |    |    |");
        console_gotoxy(45, 13);
    printf_blue("  =|      -       |=");
        console_gotoxy(45, 14);
    printf_blue("  =\\     \\/\\/     /=");
        console_gotoxy(45, 15);
    printf_blue("    \\            /");
        console_gotoxy(45, 16);
    printf_blue("     =====\\/=====");
        console_gotoxy(45, 17);
    printf_blue("        (CatK)");
        console_gotoxy(45, 18);

    read();
    if (readnum == 1)
    {
        printf("quiet");
        bootargs = "quiet";
        console_gotoxy(0, 20);
    }
    if (readnum == 2)
    {
        reboot();
    }
    console_init(COLOR_WHITE, COLOR_BLACK);
    boot();
}