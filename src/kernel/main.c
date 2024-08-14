#include <stdint.h>
#include <multiboot2.h>
#include <catk/console.h>
#include <catk/mem.h>
#include <catk/core.h>
#include <catk/errno.h>
#include <catk/printk.h>
#include <catk/io.h>
#include <catk/task.h>
#include <catk/debug.h>
#include <catk/tty.h>
#include <catk/device.h>
#include <catk/kernel.h>
#include <catk/params.h>
#include <catk/pci.h>
#include <catk/vfs.h>
#include <catk/ramdisk.h>
#include <catk/trace.h>
#include <catk/keyboard.h>
#include <catk/rand.h>
#include <lib/ctype.h>
#include <config.h>

#ifndef __GNUC__
#error "GCC or Clang please! :)"
#endif

static char * cmdline;

static bool use_hd = false; /* determines if we use a hard-disk or not */
bool kern_verbose = false;  /* set to false by default */

static void show_boot_banner(void)
{
  printk("2023-2024 The CatKernel Project.\n");
  printk("\tCreated locally in Canada, and California, bring tuques.\n");
  /* now for the long ass gpl license */
  printk("\nThis software is licensed under the GNU General Public License v3.0.\n");
  printk("Everyone is permitted to copy, distribute, and modify this software.\n\n");
}

static void show_mem_info(uintptr_t addr)
{
  struct multiboot_tag_basic_meminfo * meminfo;
  meminfo = multiboot2_locate_tag(addr, MULTIBOOT_TAG_TYPE_BASIC_MEMINFO);
  if(!meminfo)
    return;
  /* prints out memory info, just like unix :) */
  size_t total_mem = meminfo->mem_upper + meminfo->mem_lower;
  printk("real mem: %d kb\n", total_mem);
  printk("avail mem: %d kb\n",  total_mem - heap_get_used());
}

void kmain(uint32_t magic, uintptr_t addr)
{
  if(!multiboot2_validate_args(magic, addr))
    return; /* return into the infinite halt state */
  cpu_init(addr);
  heap_init();
  serial_init();
  debug(" kernel!\n");
  device_init();
  beep(10);
  int rc = console_init(addr);
  if(IS_ERR(rc)) {
    debug("Failed to initialize console: %d\n", rc);
    return;
  }
  cmdline = obtain_cmdline(addr);
  char * verbose = get_cmdline_param_val(cmdline, "verbose");
  if(verbose)
  {
    if(strcmp("true", verbose) == 0)
    {
      debug("Redirecting serial output to console...\n");
      kern_verbose = true;
    }
  }
  show_boot_banner();
  show_mem_info(addr);
  if (!cpuidcheck()) {
    panic("Could not get CPUID for this hardware!");
  }
  /* Check to see if the CPU supports CPUID. If not, panic.*/
  printk("CPU: CPUID supported\n");
  printk("Compatible CPUS are:\n\tAMD AuthenticAMD\n\tIntel GenuineIntel\n\tHygon HygonGenuine\n");
  /* Read the CPUID, once we know that it is supported.*/
  cpu_dump_all_info();
  rc = tty_create(0, get_console()->dev);
  if(rc < 0)
    panic("Could not create TTY0: %d\n", rc);
  keyboard_init();
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

#if CATK_LOGO == 1
static void show_bootart(void)
{
 /* CatK splash screen */
  printk("             __           __\n");
  printk("            /  \\         /  \\\n");
  printk("           / /\\ \\       / /\\ \\\n");
  printk("          / /  \\ \\     / /  \\ \\\n");
  printk("         / /      \\___/      \\ \\         _______   _____   _______  ___   _\n");
  printk("        /                       \\       |   ____| /  _  \\ |       ||   | | |\n");
  printk("       |        |      |         |      |  |     |  | |  ||_     _||   |_| |\n");
  printk("     ---        |      |         ---    |  |     |  |_|  |  |   |  |      _|\n");
  printk("       |                         |      |  |     |       |  |   |  |     |_ \n");
  printk("     ---   //      ^       //    ---    |  |____ |   _   |  |   |  |    _  |\n");
  printk("        \\         \\/\\/          /       |_______||__| |__|  |___|  |___| |_|\n");
  printk("         \\                     /      Written from scratch by the CatK team! :3\n");
  printk("          \\___________________/\n");
  printk("           ===================\n");
  printk("          =========\\/==========\n");
  printk("                  /  \\\n");
  printk("                 |CatK|\n");
  printk("                  \\__/\n");
  printk("\nCatK(mascot) was created by Rodmatronics\n");
}
#endif

extern int ata_find_first_partition(void);
extern int ramdisk_find_first_partition(void);

void bootstrap2(void)
{
  int rc;
#if CATK_LOGO == 1
  show_bootart();
#endif
  pci_init();
  random_init();
  /* mount rootfs */
  struct device * dev;
  dev = get_blkdev(use_hd ? DISKDEV_MAJOR : RAMDISK_MAJOR);
  if(!dev)
    panic("No drive to mount rootfs.\n");
  int first_partition_lba = use_hd ? ata_find_first_partition() : ramdisk_find_first_partition();
  rc = filesystems_init(first_partition_lba); // this will be set to a dummy value
  if(IS_ERR(rc))
    panic("Could not initialize filesystems: %d\n", rc);
  rc = vfs_init();
  if(IS_ERR(rc))
    panic("Could not initialize VFS: %d\n", rc);

  for (int i = 0; i < CATK_MOUNT_RETRIES; i++) {
      rc = vfs_mount("/", dev);
      if (!IS_ERR(rc)) {
          break;
      }

      if (!i) {
          printk("Waiting on root device...\n");
      } else {
          printk("Still waiting on root device...\n");
      }

      msleep(10000); // Wait for 10 seconds
  }

  if (IS_ERR(rc)) {
    panic("Could not mount rootfs on block (%d,%d) after %d attempts: %d.\n", MAJOR(dev->dev), MINOR(dev->dev), CATK_MOUNT_RETRIES, rc);
  }

  printk("Successfully mounted rootfs on block (%d,%d)\n", MAJOR(dev->dev), MINOR(dev->dev));
  rc = vfs_mount("/dev", dev);
  if(IS_ERR(rc))
  {
    panic("Could not mount devfs: %d\n", MAJOR(dev->dev), MINOR(dev->dev), rc);
  }
  printk("Successfully mounted devfs on block (%d,%d)\n", MAJOR(dev->dev), MINOR(dev->dev));
  /* start init process */
  rc = start_init(cmdline);
  if(IS_ERR(rc))
    panic("Failed when starting init process: %d\n", rc);
  printk("Nothing left to do. Going idle...\n");
  /* fall back to catk_idle (defined in proc/task.c:19)  */
}
