#include "read.h"
#include "config.h"

char* args;

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
    printf("| Boot normally [Enter]            |\n");
    printf("| 1.  Suppress boot logs           |\n");
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
        printf("           __           __             ");
        console_gotoxy(40, 6);
        printf("          /  \\         /  \\        ");
        console_gotoxy(40, 7);
        printf("         / /\\ \\       / /\\ \\       ");
        console_gotoxy(40, 8);
        printf("%C        / /  \\ \\     / /  \\ \\      ", 0x3, 0x0);
        console_gotoxy(40, 9);
        printf("%C       / /      \\___/      \\ \\     ", 0x3, 0x0);
        console_gotoxy(40, 10);
        printf("%C      /        \\  /           \\    ", 0x3, 0x0);
        console_gotoxy(40, 11);
        printf("%C     |        |      |         |   ", 0x3, 0x0);
        console_gotoxy(40, 12);
        printf("%C   ---        |      |         --- ", 0x3, 0x0);
        console_gotoxy(40, 13);
        printf("%C     |                         |   ", 0x3, 0x0);
        console_gotoxy(40, 14);
        printf("   ---  ");
        printf("%C//", 0x4, 0x0);
        printf("       ^       ");
        printf("%C//", 0xC, 0x0);
        printf("%C    --- ", 0x3, 0x0);
        console_gotoxy(40, 15);
        printf("%C      \\         \\/\\           /    ", 0x3, 0x0);
        console_gotoxy(40, 16);
        printf(" %C      \\                     /     ", 0x3, 0x0);
        console_gotoxy(40, 17);
        printf("%C        \\___________________/      ", 0x1, 0x0);
        console_gotoxy(40, 18);
        printf("%C         ===================       ", 0x4, 0x0);
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
