#include "read.h"
#include "config.h"
#include "kernel.h"
#include "console.h"
#include "vga.h"
#include "syspw.h"
#include "io_ports.h"

char* args;
int art = 2;

void PreBoot()
{
    vga_enable_cursor();
    console_gotoxy(0, 0);
    printf("%C _______   _____   _______  ___   _\n", 0xF, 0x0);
    printf("%C|   ____| /  _  \\ |       ||   | | |\n", 0xF, 0x0);
    printf("%C|  |     |  | |  ||_     _||   |_| |\n", 0xF, 0x0);
    printf("%C|  |     |  |_|  |  |   |  |      _|\n", 0xF, 0x0);
    printf("%C|  |     |       |  |   |  |     |_ \n", 0xF, 0x0);
    printf("%C|  |____ |   _   |  |   |  |    _  |\n", 0xF, 0x0);
    printf("%C|_______||__| |__|  |___|  |___| |_|\n\n", 0xF, 0x0);

    printf("+========== CatK PreBoot ==========+\n");
    printf("%C| Boot normally [Enter]            |\n", 0xB, 0x0);
    printf("| 1.  Suppress boot logs           |\n");
    printf("| 2.  Kitty artwork                |\n");
    printf("| 3.  Reboot                       |\n");
    printf("|                                  |\n");
    printf("|                                  |\n");
    printf("|                                  |\n");
    printf("|                                  |\n");
    printf("|                                  |\n");
    printf("+==================================+\n");

    console_gotoxy(0, 24);
    printf("%CBuild num %s", 0x8, 0x0, versionnumber);
    printf("%C  https://catk.neocities.org/", 0x8, 0x0);

    if (art == 1)
    {

        console_gotoxy(40, 5);
        printf("%C           __           __            Z ", 0xF, 0x0);
        console_gotoxy(40, 6);
        printf("%C          /  \\         /  \\        Z   ", 0xF, 0x0);
        console_gotoxy(40, 7);
        printf("%C         / /\\ \\       / /\\ \\   Z    ", 0xF, 0x0);
        console_gotoxy(40, 8);
        printf("%C        / /  \\ \\     / /  \\ \\     ", 0xB, 0x0);
        console_gotoxy(40, 9);
        printf("%C       / /      \\___/      \\ \\     ", 0xB, 0x0);
        console_gotoxy(40, 10);
        printf("%C      /                       \\    ", 0xB, 0x0);
        console_gotoxy(40, 11);
        printf("%C     |                         |   ", 0xB, 0x0);
        console_gotoxy(40, 12);
        printf("%C   ---      ----     ----      --- ", 0xB, 0x0);
        console_gotoxy(40, 13);
        printf("%C     |                         |   ", 0xB, 0x0);
        console_gotoxy(40, 14);
        printf("%C   ---  ", 0x3, 0x0);
        printf("%C//", 0x3, 0x0);
        printf("%C       ^       ", 0x3, 0x0);
        printf("%C//", 0x3, 0x0);
        printf("%C    --- ", 0x3, 0x0);
        console_gotoxy(40, 15);
        printf("%C      \\        \\\\             /    ", 0xB, 0x0);
        console_gotoxy(40, 16);
        printf("%C       \\                     /     ", 0xB, 0x0);
        console_gotoxy(40, 17);
        printf("%C        \\___________________/      ", 0x3, 0x0);
        console_gotoxy(40, 18);
        printf("%C         ===================       ", 0xC, 0x0);
        console_gotoxy(40, 19);
        printf("%C        =========", 0x4, 0x0);
        printf("%C\\/", 0xE, 0x0);
        printf("%C==========      ", 0x4, 0x0);
        console_gotoxy(40, 20);
        printf("%C                /  \\               ", 0xE, 0x0);
        console_gotoxy(40, 21);
        printf("%C               |catk|              ", 0xE, 0x0);
        console_gotoxy(40, 22);
        printf("%C                \\__/               ", 0xE, 0x0);
    }else

    if (art == 2)
    {
        console_gotoxy(40, 5);
        printf("%C           __           __             ", 0xF, 0x0);
        console_gotoxy(40, 6);
        printf("%C          /  \\         /  \\        ", 0xF, 0x0);
        console_gotoxy(40, 7);
        printf("%C         / /\\ \\       / /\\ \\       ", 0xF, 0x0);
        console_gotoxy(40, 8);
        printf("%C        / /  \\ \\     / /  \\ \\      ", 0xB, 0x0);
        console_gotoxy(40, 9);
        printf("%C       / /      \\___/      \\ \\     ", 0xB, 0x0);
        console_gotoxy(40, 10);
        printf("%C      /        \\  /           \\    ", 0xB, 0x0);
        console_gotoxy(40, 11);
        printf("%C     |        |      |         |   ", 0xB, 0x0);
        console_gotoxy(40, 12);
        printf("%C   ---        |      |         --- ", 0xB, 0x0);
        console_gotoxy(40, 13);
        printf("%C     |                         |   ", 0xB, 0x0);
        console_gotoxy(40, 14);
        printf("%C   ---  ", 0x3, 0x0);
        printf("%C//", 0x3, 0x0);
        printf("%C       ^       ", 0x3, 0x0);
        printf("%C//", 0x3, 0x0);
        printf("%C    --- ", 0x3, 0x0);
        console_gotoxy(40, 15);
        printf("%C      \\         \\/\\/          /    ", 0xB, 0x0);
        console_gotoxy(40, 16);
        printf(" %C      \\                     /     ", 0xB, 0x0);
        console_gotoxy(40, 17);
        printf("%C        \\___________________/      ", 0x3, 0x0);
        console_gotoxy(40, 18);
        printf("%C         ===================       ", 0xC, 0x0);
        console_gotoxy(40, 19);
        printf("%C        =========", 0x4, 0x0);
        printf("%C\\/", 0xE, 0x0);
        printf("%C==========      ", 0x4, 0x0);
        console_gotoxy(40, 20);
        printf("%C                /  \\               ", 0xE, 0x0);
        console_gotoxy(40, 21);
        printf("%C               |catk|              ", 0xE, 0x0);
        console_gotoxy(40, 22);
        printf("%C                \\__/               ", 0xE, 0x0);
    }else

    if (art == 3)
    {
            console_gotoxy(45, 8);
            printf("%C       _      _", 0xF, 0x0);
            console_gotoxy(45, 9);
            printf("%C      / \\    / \\", 0xF, 0x0);
            console_gotoxy(45, 10);
            printf("%C     /   \\__/   \\", 0xB, 0x0);
            console_gotoxy(45, 11);
            printf("%C    /            \\", 0xB, 0x0);
            console_gotoxy(45, 12);
            printf("%C   |    |    |    |", 0xB, 0x0);
            console_gotoxy(45, 13);
            printf("%C  =|      -       |=", 0xB, 0x0);
            console_gotoxy(45, 14);
            printf("%C  =\\     \\/\\/     /=", 0xB, 0x0);
            console_gotoxy(45, 15);
            printf("%C    \\            /", 0x3, 0x0);
            console_gotoxy(45, 16);
            printf("%C     =====\\/=====", 0xC, 0x0);
            console_gotoxy(45, 17);
            printf("%C        (CatK)", 0xE, 0x0);
            console_gotoxy(45, 18);
    }

    read(0);

    if (readnum == 1)
    {
        printf("quiet");
        bootargs = "quiet";
        console_gotoxy(0, 20);
    }

    if (readnum == 2)
    {
        console_init(COLOR_BRIGHT_CYAN, COLOR_BLACK);
        console_gotoxy(0, 0);
        printf("%C _______   _____   _______  ___   _\n", 0xF, 0x0);
        printf("%C|   ____| /  _  \\ |       ||   | | |\n", 0xF, 0x0);
        printf("%C|  |     |  | |  ||_     _||   |_| |\n", 0xF, 0x0);
        printf("%C|  |     |  |_|  |  |   |  |      _|\n", 0xF, 0x0);
        printf("%C|  |     |       |  |   |  |     |_ \n", 0xF, 0x0);
        printf("%C|  |____ |   _   |  |   |  |    _  |\n", 0xF, 0x0);
        printf("%C|_______||__| |__|  |___|  |___| |_|\n\n", 0xF, 0x0);

        printf("%C+========== CatK PreBoot ==========+\n", 0x8, 0x0);
        printf("%C| Boot normally [Enter]            |\n", 0x8, 0x0);
        printf("%C| 1.  Suppress boot logs           |\n", 0x8, 0x0);
        printf("%C| Kitty artwork                    |\n", 0xB, 0x0);
        printf("%C| 3.  Reboot                       |\n", 0x8, 0x0);
        printf("%C|                                  |\n", 0x8, 0x0);
        printf("%C|                                  |\n", 0x8, 0x0);
        printf("%C|                                  |\n", 0x8, 0x0);
        printf("%C|                                  |\n", 0x8, 0x0);
        printf("%C|                                  |\n", 0x8, 0x0);
        printf("%C+==================================+\n", 0x8, 0x0);

        console_gotoxy(46, 10);
        printf("1.) Big Sleepy kitty");
        console_gotoxy(46, 11);
        printf("2.) Big kitty");
        console_gotoxy(46, 12);
        printf("3.) Small kitty");
        console_gotoxy(46, 14);
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
    
    args = "none";
    boot();

}
