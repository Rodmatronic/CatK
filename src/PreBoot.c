#include "read.h"
#include "config.h"
#include "kernel.h"
#include "console.h"
#include "vga.h"
#include "io_ports.h"

char* args;

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
    printf("|                                  |\n");
    printf("|                                  |\n");
    printf("|                                  |\n");
    printf("|                                  |\n");
    printf("|                                  |\n");
    printf("|                                  |\n");
    printf("|                                  |\n");
    printf("|                                  |\n");
    printf("+==================================+\n");

    console_gotoxy(0, 24);
    printf("%CBuild num %s", 0x8, 0x0, versionnumber);
    printf("%C  https://catk.neocities.org/", 0x8, 0x0);

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

    read(1);

    args = "none";

    boot();

}
