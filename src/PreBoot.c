int art = 2;
int initdebug = 0;
int skipinit = 0;

void vga_enable_cursor() {
    outportb(0x3D4, 0x0A);
    outportb(0x3D5, (inportb(0x3D5) & 0xC0) | 0);
    outportb(0x3D4, 0x0B);
    outportb(0x3D5, (inportb(0x3D5) & 0xE0) | 0x0E);  // Set the end scanline to 14 and disable blink
}

void PreBoot()
{
    vga_enable_cursor();
    console_gotoxy(0, 0);
    printf(" _______   _____   _______  ___   _\n");
    printf("|   ____| /  _  \\ |       ||   | | |\n");
    printf("|  |     |  | |  ||_     _||   |_| |\n");
    printf("|  |     |  |_|  |  |   |  |      _|\n");
    printf("|  |     |       |  |   |  |     |_ \n");
    printf("|  |____ |   _   |  |   |  |    _  |\n");
    printf("|_______||__| |__|  |___|  |___| |_|\n\n");

    printf("+==================================+\n");
    printf("|    CatK PreBoot                  |\n");
    printf_blue("| Boot normally [Enter]            |\n");
    printf("| 1.  Suppress boot logs           |\n");
    printf("| 2.  Kitty artwork                |\n");
    printf("| 3.  Reboot                       |\n");
    printf("|------------------------          |\n");
    printf("| 4.  Advanced (careful!)          |\n");
    printf("|                                  |\n");
    printf("|                                  |\n");
    printf("+==================================+\n");

    console_gotoxy(0, 24);
    printf_dark("Build num %s", versionnumber);
    printf_dark("  https://catk.neocities.org/");

    if (art == 1)
    {
        console_gotoxy(40, 5);
        printf_white("           __           __");
        printf_darkblue("          Z    ");
        console_gotoxy(40, 6);
        printf_white("          /  \\         /  \\");
        printf_darkblue("      Z    ");
        console_gotoxy(40, 7);
        printf_blue("         / /\\ \\       / /\\ \\");
        printf_darkblue("  Z    ");
        console_gotoxy(40, 8);
        printf_blue("        / /  \\ \\     / /  \\ \\");
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
        printf_blue("   ---  ");
        printf_darkblue("//");
        printf_blue("       ^       ");
        printf_darkblue("//");
        printf_blue("    --- ");
        console_gotoxy(40, 15);
        printf_blue("      \\         \\\\            /    ");
        console_gotoxy(40, 16);
        printf_blue("       \\                     /     ");
        console_gotoxy(40, 17);
        printf_darkblue("        \\___________________/      ");
        console_gotoxy(40, 18);
        printf_red("         ===================       ");
        console_gotoxy(40, 19);
        printf_red("        =========");
        printf_yellow("\\/");
        printf_red("==========      ");
        console_gotoxy(40, 20);
        printf_yellow("                /  \\               ");
        console_gotoxy(40, 21);
        printf_yellow("               |catk|              ");
        console_gotoxy(40, 22);
        printf_yellow("                \\__/               ");
    }else

    if (art == 2)
    {
        console_gotoxy(40, 5);
        printf_white("           __           __             ");
        console_gotoxy(40, 6);
        printf_white("          /  \\         /  \\        ");
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
        printf_blue("   ---  ");
        printf_darkblue("//");
        printf_blue("       ^       ");
        printf_darkblue("//");
        printf_blue("    --- ");
        console_gotoxy(40, 15);
        printf_blue("      \\         \\/\\/          /    ");
        console_gotoxy(40, 16);
        printf_blue("       \\                     /     ");
        console_gotoxy(40, 17);
        printf_darkblue("        \\___________________/      ");
        console_gotoxy(40, 18);
        printf_red("         ===================       ");
        console_gotoxy(40, 19);
        printf_red("        =========");
        printf_yellow("\\/");
        printf_red("==========      ");
        console_gotoxy(40, 20);
        printf_yellow("                /  \\               ");
        console_gotoxy(40, 21);
        printf_yellow("               |catk|              ");
        console_gotoxy(40, 22);
        printf_yellow("                \\__/               ");
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


    read(1);
    if (readnum == -1)
    {
        console_init(COLOR_GREY, COLOR_BLACK);
        PreBoot();
    }
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
        read(1);
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

    if (readnum == 4)
    {
        console_init(COLOR_WHITE, COLOR_BLACK);

        console_gotoxy(45, 12);
        printf_blue("        /--        --\\");
        console_gotoxy(45, 13);
        printf_blue("      /   |        |   \\");
        console_gotoxy(45, 14);
        printf_blue("    /     |        |     \\");
        console_gotoxy(45, 15);
        printf_blue("   /      |        |      \\");
        console_gotoxy(45, 16);
        printf_blue("  /       |        |       \\");
        console_gotoxy(45, 17);
        printf_blue(" |        |________|        |");
        console_gotoxy(45, 18);
        printf_blue(" |                          |");
        console_gotoxy(45, 19);
        printf_blue(" |                          |");
        console_gotoxy(45, 20);
        printf_blue("  \\       CATK DEBUG       / ");
        console_gotoxy(45, 21);
        printf_blue("   \\                      /  ");
        console_gotoxy(45, 22);
        printf_blue("     \\                  /    ");
        console_gotoxy(45, 23);
        printf_blue("        \\             /    ");
        console_gotoxy(45, 24);
        printf_blue("         |           |    ");
        console_gotoxy(45, 25);
        printf_blue("         |           |    ");

        console_gotoxy(0, 0);
        printf("Advanced debugging stuffs. CAREFUL!\n");
        printf("1) Back\n");
        printf("2) Force broke init\n");
        printf("3) Skip init entireley (Leads to problems!)\n");

        read(1);
        if (readnum == -1)
        {
            console_init(COLOR_GREY, COLOR_BLACK);
            PreBoot();
        }
        if (readnum == 1)
        {
            console_init(COLOR_GREY, COLOR_BLACK);
            PreBoot();
        }

        if (readnum == 2)
        {
            initdebug = 1;
        }

        if (readnum == 3)
        {
            skipinit = 1;
        }
    }
    console_init(COLOR_WHITE, COLOR_BLACK);
    boot();
}