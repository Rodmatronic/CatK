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
#include <catk/kernel.h>
#include <catk/pci.h>
#include <catk/vfs.h>
#include <catk/ramdisk.h>
#include <catk/trace.h>
#include <lib/ctype.h>

extern uintptr_t kernel_start;
extern uintptr_t kernel_end;

static char * cmdline;

static bool use_hd = false; /* determines if we use a hard-disk or not */
static void show_bootart(void);

extern int cpudetect();

static void show_boot_banner(void)
{
  printk("2023-2024 The CatKernel Project.\n");
  printk("\tCreated locally in Canada, and California, bring tuques.\n");
  /* now for the long ass gpl license */
  printk("\nThis software is licensed under the GNU General Public License v3.0.\n");
  printk("Everyone is permitted to copy, distribute, and modify this software.\n\n");
}

void cpuid(uint32_t code, uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d) {
    __asm__ volatile("cpuid"
                     : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
                     : "a"(code));
}

int cpuget() {
  uint32_t eax, ebx, ecx, edx;
  
  // Call cpuid with code 0 to get the vendor ID string
  cpuid(0, &eax, &ebx, &ecx, &edx);

  char vendor[13];
  *((uint32_t *)vendor) = ebx;
  *((uint32_t *)(vendor + 4)) = edx;
  *((uint32_t *)(vendor + 8)) = ecx;
  vendor[12] = '\0';

  printk("CPUID is: %s\n", vendor);
  return 1;
}

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
  cmdline = obtain_cmdline(addr);
  show_boot_banner();
  printk("CatK cmdline: %s\n", cmdline);
  
  if (!cpudetect()) {
      panic("Invalid CPU/could not get CPUID for this hardware!");
  }
  /* Check to see if the CPU supports CPUID. If not, panic.*/
  printk("CPU: CPUID supported\n");
  /* Read the CPUID, once we know that it is supported.*/
  cpuget();
  rc = tty_create(0, get_console()->dev);
  if(rc < 0)
    panic("Could not create TTY0: %d\n", rc);
  rc = ramdisk_probe(addr);
  if (IS_ERR(rc))
  {
    printk("No ramdisk loaded, defaulting to hard-disk...\n");
    use_hd = true;
  }
  tasking_init();
  /* it is impossible for tasking_init to return */
  panic("Failed to init tasks, kernel left in unreachable state");
}

extern int ata_find_first_partition(void);
extern int ramdisk_find_first_partition(void);

void bootstrap2(void)
{
  debug("[kernel] %s start\n", __FUNCTION__);
  int rc, attempts;
  show_bootart();
  pci_init();
  /* mount rootfs */
  struct device * dev;
  if(use_hd)
    dev = get_blkdev(DISKDEV_MAJOR);
  else
    dev = get_blkdev(RAMDISK_MAJOR);
  if(!dev)
    panic("No drive to mount rootfs.\n");
  int first_partition_lba = use_hd ? ata_find_first_partition() : ramdisk_find_first_partition();
  rc = filesystems_init(first_partition_lba); // this will be set to a dummy value
  if(IS_ERR(rc))
    panic("Could not initialize filesystems: %d\n", rc);
  rc = vfs_init();
  if(IS_ERR(rc))
    panic("Could not initialize VFS: %d\n", rc);
  rc = vfs_mount("/", dev);
  if(IS_ERR(rc))
  {
    panic("Could not mount rootfs on block (%d,%d): %d\n", dev->major, dev->minors, rc);
  }
  printk("Successfully mounted rootfs on block (%d,%d)\n", dev->major, dev->minors);
  rc = vfs_mount("/dev", dev);
  if(IS_ERR(rc))
  {
    panic("Could not mount devfs: %d\n", dev->major, dev->minors, rc);
  }
  printk("Successfully mounted devfs on block (%d,%d)\n", dev->major, dev->minors);
  /* start init process */
  rc = start_init(cmdline);
  if(IS_ERR(rc))
    panic("Failed when starting init process: %d\n", rc);
  printk("Nothing left to do. Going idle...\n");
  /* fall back to catk_idle (defined in proc/task.c:19)  */
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
