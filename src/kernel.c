#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <term.h>
#include "term.h"
#include "printk.h"
#include "panic.h"

void bootart();

void kmain() 
{
	terminal_init();
	printk("%CCat... ", VGA_COLOR_CYAN);
	printk("%Ckernel!\n", VGA_COLOR_LIGHT_CYAN);
	bootart();
}

void bootart()
{
        printk("%C           __           __             \n",VGA_COLOR_WHITE);
        printk("%C          /  \\         /  \\        \n", VGA_COLOR_WHITE);
        printk("%C         / /\\ \\       / /\\ \\       \n", VGA_COLOR_LIGHT_CYAN);
        printk("%C        / /  \\ \\     / /  \\ \\      \n", VGA_COLOR_LIGHT_CYAN);
        printk("%C       / /      \\___/      \\ \\     \n", VGA_COLOR_LIGHT_CYAN);
        printk("%C      /                       \\    \n", VGA_COLOR_LIGHT_CYAN);
        printk("%C     |        |      |         |   \n", VGA_COLOR_LIGHT_CYAN);
        printk("%C   ---        |      |         --- \n", VGA_COLOR_LIGHT_CYAN);
        printk("%C     |                         |   \n", VGA_COLOR_LIGHT_CYAN);
        printk("%C   ---  ", VGA_COLOR_LIGHT_CYAN);
        printk("%C//", VGA_COLOR_CYAN);
        printk("%C       ^       ", VGA_COLOR_LIGHT_CYAN);
        printk("%C//", VGA_COLOR_CYAN);
        printk("%C    --- \n", VGA_COLOR_LIGHT_CYAN);
        printk("%C      \\         \\/\\/          /    \n", VGA_COLOR_LIGHT_CYAN);
        printk(" %C      \\                     /     \n", VGA_COLOR_CYAN);
        printk("%C        \\___________________/      \n", VGA_COLOR_CYAN);
        printk("%C         ===================       \n", VGA_COLOR_LIGHT_RED);
        printk("%C        =========", VGA_COLOR_RED);
        printk("%C\\/", VGA_COLOR_LIGHT_BROWN);
        printk("%C==========      \n", VGA_COLOR_RED);
        printk("%C                /  \\               \n", VGA_COLOR_LIGHT_BROWN);
        printk("%C               |catk|              \n", VGA_COLOR_LIGHT_BROWN);
        printk("%C                \\__/               \n", VGA_COLOR_LIGHT_BROWN);
}