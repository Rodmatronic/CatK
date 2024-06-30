#include <stdint.h>
#include <multiboot2.h>
#include <catk/console.h>
#include <catk/mem.h>
#include <catk/core.h>
#include <catk/errno.h>
#include <catk/printk.h>
#include <catk/task.h>
#include <catk/debug.h>
#include <catk/tty.h>
#include <lib/ctype.h>

extern uintptr_t kernel_start;
extern uintptr_t kernel_end;

static void show_bootart(void);

void kmain(uint32_t magic, uintptr_t addr)
{
  if(!multiboot2_validate_args(magic, addr))
    return; /* return into the infinite halt state */
  cpu_init();
  heap_init(&kernel_end);
  int rc = console_init(addr);
  if(IS_ERR(rc))
    return;
  show_bootart();
  serial_init();
  debug(" ..Kernel!\n");
  rc = tty_create(0, get_console()->dev);
  if(rc < 0)
    panic("Could not create TTY0: %d\n", rc);
  tasking_init();
  /* it is impossible for tasking_init to return */
  unreachable;
}

static void bobs_task(void)
{
  tty_write(tty_lookup(0), (uint8_t *)"Hi!", 3);
  for(;;);
}

void bootstrap2(void)
{
  printk("\033[1;37mStarting Catk in 3...\033[1;0m\n");
  spawn_kernel_task("Bob", (uint32_t)bobs_task, TASK_PRIORITY_NORMAL);
  for(;;);
}

static void show_bootart(void)
{
  /* CatK splash screen */
  printk("\n\033[1;36m           __           __             \n");
  printk("          /  \\         /  \\        \n");
  printk("         / /\\ \\       / /\\ \\       \n");
  printk("        / /  \\ \\     / /  \\ \\      \n");
  printk("       / /      \\___/      \\ \\         _______   _____   _______  ___   _\n");
  printk("      /                       \\       |   ____| /  _  \\ |       ||   | | |\n");
  printk("     |        |      |         |      |  |     |  | |  ||_     _||   |_| |\n");
  printk("   ---        |      |         ---    |  |     |  |_|  |  |   |  |      _|\n");
  printk("     |                         |      |  |     |       |  |   |  |     |_ \n");
  printk("   ---  ");
  printk("\033[1;36m//\033[1;36m");
  printk("       ^       ");
  printk("\033[1;36m//\033[1;36m");
  printk("    ---    |  |____ |   _   |  |   |  |    _  |\n");
  printk("      \\         \\/\\/          /       \033[1;36m|_______||__| |__|  |___|  |___| |_|\033[1;36m\n");
  printk("\033[1;36m       \\                     /        Written from scratch by the CatK team! :3\n");
  printk("        \\___________________/      \n");
  printk("\033[31m         ===================       \n");
  printk("\033[1;31m        =========");
  printk("\033[33m\\/\033[31m");
  printk("==========      \n");
  printk("\033[33m                /  \\               \n");
  printk("               |CatK|              \n");
  printk("                \\__/               \033[1;0m\n");
  printk("\n\n\nCatK(mascot) was created by Rodmatronics\n");
}
