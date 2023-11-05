int art = 2;

void PreBoot()
{
    console_gotoxy(0, 0);
    printf(" _______   _____   _______  ___   _\n");
    printf("|   ____| /  _  \\ |       ||   | | |\n");
    printf("|  |     |  | |  ||_     _||   |_| |\n");
    printf("|  |     |  |_|  |  |   |  |      _|\n");
    printf("|  |     |       |  |   |  |     |_ \n");
    printf("|  |____ |   _   |  |   |  |    _  |\n");
    printf("|_______||__| |__|  |___|  |___| |_|\n\n");

    printf("+==================================+\n");
    printf("|    CatK PreBoot\n");
    printf_blue("| Boot normally [Enter]\n");
    printf("| 1.  Suppress boot logs\n");
    printf("| 2.  Kitty artwork\n");
    printf("| 3.  Reboot\n");
    printf("|\n");
    printf("|\n");
    printf("|\n");
    printf("|\n");
    printf("+==================================+\n");

    console_gotoxy(0, 24);
    printf_dark("Build num %s", versionnumber);
    printf_dark("  https://catk.neocities.org/");

    if (art == 1)
    {
        console_gotoxy(40, 5);
        printf_blue("           __           __            Z ");
        console_gotoxy(40, 6);
        printf_blue("          /  \\         /  \\        Z");
        console_gotoxy(40, 7);
        printf_blue("         / /\\ \\       / /\\ \\   Z    ");
        console_gotoxy(40, 8);
        printf_blue("        / /  \\ \\     / /  \\ \\      ");
        console_gotoxy(40, 9);
        printf_blue("       / /      \\___/      \\ \\     ");
        console_gotoxy(40, 10);
        printf_blue("      /                       \\    ");
        console_gotoxy(40, 11);
        printf_blue("     |                         |   ");
        console_gotoxy(40, 12);
        printf_blue("   ---      ----     ----      --- ");
        console_gotoxy(40, 13);
        printf_blue("     |                         |   ");
        console_gotoxy(40, 14);
        printf_blue("   ---  //       ^       //    --- ");
        console_gotoxy(40, 15);
        printf_blue("      \\         \\\\            /    ");
        console_gotoxy(40, 16);
        printf_blue("       \\                     /     ");
        console_gotoxy(40, 17);
        printf_blue("        \\___________________/      ");
        console_gotoxy(40, 18);
        printf_blue("        =====================      ");
        console_gotoxy(40, 19);
        printf_blue("        =========\\/==========      ");
        console_gotoxy(40, 20);
        printf_blue("                /  \\               ");
        console_gotoxy(40, 21);
        printf_blue("               |catk|              ");
        console_gotoxy(40, 22);
        printf_blue("                \\__/               ");
    }else

    if (art == 2)
    {
        console_gotoxy(40, 5);
        printf_blue("           __           __             ");
        console_gotoxy(40, 6);
        printf_blue("          /  \\         /  \\        ");
        console_gotoxy(40, 7);
        printf_blue("         / /\\ \\       / /\\ \\       ");
        console_gotoxy(40, 8);
        printf_blue("        / /  \\ \\     / /  \\ \\      ");
        console_gotoxy(40, 9);
        printf_blue("       / /      \\___/      \\ \\     ");
        console_gotoxy(40, 10);
        printf_blue("      /                       \\    ");
        console_gotoxy(40, 11);
        printf_blue("     |        |      |         |   ");
        console_gotoxy(40, 12);
        printf_blue("   ---        |      |         --- ");
        console_gotoxy(40, 13);
        printf_blue("     |                         |   ");
        console_gotoxy(40, 14);
        printf_blue("   ---  //       ^       //    --- ");
        console_gotoxy(40, 15);
        printf_blue("      \\         \\/\\/          /    ");
        console_gotoxy(40, 16);
        printf_blue("       \\                     /     ");
        console_gotoxy(40, 17);
        printf_blue("        \\___________________/      ");
        console_gotoxy(40, 18);
        printf_blue("        =====================      ");
        console_gotoxy(40, 19);
        printf_blue("        =========\\/==========      ");
        console_gotoxy(40, 20);
        printf_blue("                /  \\               ");
        console_gotoxy(40, 21);
        printf_blue("               |catk|              ");
        console_gotoxy(40, 22);
        printf_blue("                \\__/               ");
    }else

    if (art == 3)
    {
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
    }


    read();
    if (readnum == 1)
    {
        printf("quiet");
        bootargs = "quiet";
        console_gotoxy(0, 20);
    }
    if (readnum == 2)
    {
        console_init(COLOR_BRIGHT_CYAN, COLOR_BLACK);
        printf("1: Big sleepy kitty \n2: Big kitty \n3: Small kitty \n\n");
        read();
        if (readnum == 1)
        {
            art = 1;
        }else
        if (readnum == 2)
        {
            art = 2;
        }else
        if (readnum == 3)
        {
            art = 3;
        }
        console_init(COLOR_GREY, COLOR_BLACK);
        readnum = 0;
        PreBoot();
    }
    if (readnum == 3)
    {
        syspw(0);
    }
    console_init(COLOR_WHITE, COLOR_BLACK);
    boot();
}