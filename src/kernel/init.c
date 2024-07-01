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
#include <catk/device.h>
#include <catk/pci.h>
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
  serial_init();
  debug(" kernel!\n");
  device_init();
  int rc = console_init(addr);
  if(IS_ERR(rc))
    return;
  rc = tty_create(0, get_console()->dev);
  if(rc < 0)
    panic("Could not create TTY0: %d\n", rc);
  tasking_init();
  /* it is impossible for tasking_init to return */
  panic("Failed to init tasks, kernel left in unreachable state");
}

extern int ata_device_probe(void);

void bootstrap2(void)
{
  int rc;
  show_bootart();
  pci_init();
  /*
  struct device * dev = get_blkdev(DISKDEV_MAJOR);
  if(!dev)
    panic("No drive to mount rootfs.\n");
  rc = -1;
  if(IS_ERR(rc))
    panic("Could not mount rootfs on block (%d,%d)\n", dev->major, dev->minors);
  */
  printk("Nothing left to do. Going idle...\n");
  for(;;);
}

static void show_bootart(void)
{
  /* CatK splash screen */
  printk("\n\033[1;37m           __           __             \n");
  printk("          /  \\         /  \\        \n");
  printk("\033[36m         / /\\ \\       / /\\ \\       \n");
  printk("\033[36m        / /  \\ \\     / /  \\ \\      \n");
  printk("\033[36m       / /      \\___/      \\ \\      \033[1;37m   _______   _____   _______  ___   _\n");
  printk("\033[36m      /                       \\       \033[1;37m|   ____| /  _  \\ |       ||   | | |\n");
  printk("\033[36m     |        |      |         |      \033[36m|  |     |  | |  ||_     _||   |_| |\n");
  printk("\033[36m   ---        |      |         ---    |  |     |  |_|  |  |   |  |      _|\n");
  printk("     |                         |      |  |     |       |  |   |  |     |_ \n");
  printk("\033[1;36m   ---   //      ^       //    ---    \033[1;36m|  |____ |   _   |  |   |  |    _  |\n");
  printk("      \\         \\/\\/          /       \033[1;36m|_______||__| |__|  |___|  |___| |_|\033[1;36m\n");
  printk("\033[1;36m       \\                     /      Written from scratch by the CatK team! :3\n");
  printk("        \\___________________/      \n");
  printk("\033[1;31m         ===================       \n");
  printk("\033[1;31m        =========");
  printk("\033[1;33m\\/\033[1;31m");
  printk("==========      \n");
  printk("\033[1;33m                /  \\               \n");
  printk("               |CatK|              \n");
  printk("                \\__/               \033[1;0m\n");
  printk("\nCatK(mascot) was created by Rodmatronics\n");
}
