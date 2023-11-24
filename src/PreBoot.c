int art = 2;
int initdebug = 0;
int skipinit = 0;
int fillfs = 0;

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

    printf("+========== CatK PreBoot ==========+\n");
    printf_brightcyan("| Boot normally [Enter]            |\n");
    printf("| 1.  Suppress boot logs           |\n");
    printf("| 2.  Kitty artwork                |\n");
    printf("| 3.  Reboot                       |\n");
    printf("|------------------------          |\n");
    printf("| 4.  Advanced (careful!)          |\n");
    printf("|                                  |\n");
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
        printf_darkcyan("          Z    ");
        console_gotoxy(40, 6);
        printf_white("          /  \\         /  \\");
        printf_brightcyan("      Z    ");
        console_gotoxy(40, 7);
        printf_brightcyan("         / /\\ \\       / /\\ \\");
        printf_darkcyan("  Z    ");
        console_gotoxy(40, 8);
        printf_brightcyan("        / /  \\ \\     / /  \\ \\");
        console_gotoxy(40, 9);
        printf_brightcyan("       / /      \\___/      \\ \\     ");
        console_gotoxy(40, 10);
        printf_brightcyan("      /                       \\    ");
        console_gotoxy(40, 11);
        printf_brightcyan("     |                         |   ");
        console_gotoxy(40, 12);
        printf_brightcyan("   ---      ----     ----      --- ");
        console_gotoxy(40, 13);
        printf_brightcyan("     |                         |   ");
        console_gotoxy(40, 14);
        printf_brightcyan("   ---  ");
        printf_darkcyan("//");
        printf_brightcyan("       ^       ");
        printf_darkcyan("//");
        printf_brightcyan("    --- ");
        console_gotoxy(40, 15);
        printf_brightcyan("      \\         \\\\            /    ");
        console_gotoxy(40, 16);
        printf_brightcyan("       \\                     /     ");
        console_gotoxy(40, 17);
        printf_darkcyan("        \\___________________/      ");
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
        printf_brightcyan("         / /\\ \\       / /\\ \\       ");
        console_gotoxy(40, 8);
        printf_brightcyan("        / /  \\ \\     / /  \\ \\      ");
        console_gotoxy(40, 9);
        printf_brightcyan("       / /      \\___/      \\ \\     ");
        console_gotoxy(40, 10);
        printf_brightcyan("      /                       \\    ");
        console_gotoxy(40, 11);
        printf_brightcyan("     |        |      |         |   ");
        console_gotoxy(40, 12);
        printf_brightcyan("   ---        |      |         --- ");
        console_gotoxy(40, 13);
        printf_brightcyan("     |                         |   ");
        console_gotoxy(40, 14);
        printf_brightcyan("   ---  ");
        printf_darkcyan("//");
        printf_brightcyan("       ^       ");
        printf_darkcyan("//");
        printf_brightcyan("    --- ");
        console_gotoxy(40, 15);
        printf_brightcyan("      \\         \\/\\/          /    ");
        console_gotoxy(40, 16);
        printf_brightcyan("       \\                     /     ");
        console_gotoxy(40, 17);
        printf_darkcyan("        \\___________________/      ");
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
            console_gotoxy(45, 8);
            printf_white("       _      _");
            console_gotoxy(45, 9);
            printf_white("      / \\    / \\");
            console_gotoxy(45, 10);
            printf_brightcyan("     /   \\__/   \\");
            console_gotoxy(45, 11);
            printf_brightcyan("    /            \\");
            console_gotoxy(45, 12);
            printf_brightcyan("   |    |    |    |");
            console_gotoxy(45, 13);
            printf_brightcyan("  =|      -       |=");
            console_gotoxy(45, 14);
            printf_brightcyan("  =\\     \\/\\/     /=");
            console_gotoxy(45, 15);
            printf_brightcyan("    \\            /");
            console_gotoxy(45, 16);
            printf_red("     =====\\/=====");
            console_gotoxy(45, 17);
            printf_yellow("        (CatK)");
            console_gotoxy(45, 18);
    }

    if (art == 4)
    {
            console_gotoxy(45, 8);
            printf_brown("      /\\-\\___/--/\\");
            console_gotoxy(45, 9);
            printf_brown("     /__/     \\___\\");
            console_gotoxy(45, 10);
            printf_brown("      |          \\");
            console_gotoxy(45, 11);
            printf_brown("   ___| |  |     \\");
            console_gotoxy(45, 12);
            printf_brown("  /##             |");
            console_gotoxy(45, 13);
            printf_brown(" | #      //      |");
            console_gotoxy(45, 14);
            printf_brown(" |_|__/           |");
            console_gotoxy(45, 15);
            printf_brown("  \\______        /");
            console_gotoxy(45, 16);
            printf_brown("         /       \\ ");
            console_gotoxy(45, 18);
    }


    if (art == 5)
    {
            console_gotoxy(43, 6);
            printf_red("  '''");
            printf_red("                       '");
            console_gotoxy(43, 7);
            printf_brightred(" s' '.....---....--.'''     -/");
            console_gotoxy(43, 8);
            printf_brightred(" +o   .--'        /y:'      +.");
            console_gotoxy(43, 9);
            printf_brightred("  yo':.           :o      '+-");
            console_gotoxy(43, 10);
            printf_brightred("  y/               -/'   -o/");
            console_gotoxy(43, 11);
            printf_brightred(" .-                  ::/sy+:.");
            console_gotoxy(43, 12);
            printf_brightred(" /                     '--  /");
            console_gotoxy(43, 13);
            printf_brightred("':                          :'");
            console_gotoxy(43, 14);
            printf_brightred("':                          :'");
            console_gotoxy(43, 15);
            printf_brightred(" /                          /");
            console_gotoxy(43, 16);
            printf_brightred(" .-                        -.");
            console_gotoxy(43, 17);
            printf_brightred("  --                      -.");
            console_gotoxy(43, 18);
            printf_brightred("   ':'                  ':' ");
            console_gotoxy(43, 19);
            printf_brightred("     .--             '--.");
            console_gotoxy(43, 20);
            printf_brightred("        .---.....----.");
            console_gotoxy(43, 21);
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
        printf("1: Big sleepy kitty \n2: Big kitty \n3: Small kitty \n");
        printf_brown("4: Doggy \n");
        printf_brightred("5: BSD sphere \n\n");
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
        if (readnum == 4)
        {
            art = 4;
        }
        if (readnum == 5)
        {
            art = 5;
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
        printf_white("        /--        --\\");
        console_gotoxy(45, 13);
        printf_white("      /   |        |   \\");
        console_gotoxy(45, 14);
        printf_brightcyan("    /     |        |     \\");
        console_gotoxy(45, 15);
        printf_brightcyan("   /      |        |      \\");
        console_gotoxy(45, 16);
        printf_brightcyan("  /       |        |       \\");
        console_gotoxy(45, 17);
        printf_brightcyan(" |        |________|        |");
        console_gotoxy(45, 18);
        printf_brightcyan(" |                          |");
        console_gotoxy(45, 19);
        printf_brightcyan(" |                          |");
        console_gotoxy(45, 20);
        printf_brightcyan("  \\       CATK DEBUG       / ");
        console_gotoxy(45, 21);
        printf_brightcyan("   \\                      /  ");
        console_gotoxy(45, 22);
        printf_darkcyan("     \\                  /    ");
        console_gotoxy(45, 23);
        printf_darkcyan("        \\             /    ");
        console_gotoxy(45, 24);
        printf_darkcyan("         |           |    ");
        console_gotoxy(45, 25);
        printf_darkcyan("         |           |    ");

        console_gotoxy(0, 0);
        printf("Advanced debugging stuffs. CAREFUL!\n");
        printf("1) Back\n");
        printf("2) Force broke init\n");
        printf("3) Skip init entireley (Leads to problems!)\n");
        printf("4) Fill filesystem (ruh roh!)\n");

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

        if (readnum == 4)
        {
            fillfs = 1;
        }
    }
    boot();
}