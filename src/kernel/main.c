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
#include <catk/trace.h>
#include <catk/keyboard.h>
#include <catk/rand.h>
#include <catk/initrd.h>
#include <catk/module.h>
#include <lib/ctype.h>
#include <config.h>

#ifndef __GNUC__
#error "Compile with GCC or Clang please! :)"
#endif

static char * cmdline;

static void show_boot_banner(void) {
  printk("2023-2024 The CatKernel Project.\n");
  printk("\tCreated locally in Canada, and California, bring tuques and cold drinks.\n");
  printk("\nThis software is licensed under the GNU General Public License v3.0.\n");
  printk("Everyone is permitted to copy, distribute, and modify this software.\n\n");
}

void kmain(uint32_t magic, uintptr_t mbi) {
  if(!multiboot2_validate_args(magic, mbi)) {
    debug("Bootloader sent us with a bad multiboot2 information. Off to the kitty void, we go! :)\n");
    return; /* return into the infinite halt state */
  }
  multiboot2_set_mbi(mbi);
  cpu_init();
  physmem_init();
  beep(10);
  int rc = console_init();
  if(IS_ERR(rc)) {
    debug("Failed to initialize console: %d\n", rc);
    return;
  }
  cmdline = obtain_cmdline(mbi);
  show_boot_banner();
  if (!cpuidcheck()) {
    panic("Could not get CPUID for this hardware!");
  }
  /* Check to see if the CPU supports CPUID. If not, panic.*/
  printk("CPU: CPUID supported\n");
  printk("Compatible CPUS are:\n\tAMD AuthenticAMD\n\tIntel GenuineIntel\n\tHygon HygonGenuine\n");
  /* Read the CPUID, once we know that it is supported.*/
  cpu_dump_all_info();
  rc = tty_create(0, console_get(0)->dev);
  if(rc < 0)
    panic("Could not create TTY0: %d\n", rc);
  builtin_modules_init();
  keyboard_init();
  initrd_probe();
  tasking_init();
  /* it is impossible for tasking_init to return */
  panic("Failed to init tasks, kernel left in unreachable state");
  unreachable;
}

/*
static void show_bootart(void)
{
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
}*/

void bootstrap2(void) {
  int rc;
  pci_init();
  random_init();
  /* find first available block device */
  struct device * dev = blkdev_get_first();
  if(!dev)
    panic("No drive to mount rootfs.\n");
  int first_part = dev->fops->firstpart();
  rc = filesystems_init(first_part); // this will be set to a dummy value
  if(IS_ERR(rc))
    panic("Could not initialize filesystems: %d\n", rc);
  for (int i = 0; i < CATK_MOUNT_RETRIES; i++) {
      struct filesystem * fs = (i % 2) == 0 ? get_filesystem("ext2") : get_filesystem("ustar");
      rc = vfs_mount("/", dev, fs);
      if (!IS_ERR(rc)) {
          vfs_set_rootfs(fs);
          break;
      }

      if (i == 0) {
          printk("Waiting on root device...\n");
      } else {
          printk("Still waiting on root device...\n");
      }

      msleep(CATK_REMOUNT_DELAY * 1000); // Wait for 10 seconds
  }

  if (IS_ERR(rc)) {
    panic("Could not mount rootfs on block (%d,%d) after %d attempts: %d.\n", MAJOR(dev->dev), MINOR(dev->dev), CATK_MOUNT_RETRIES, rc);
  }

  printk("Successfully mounted rootfs on block (%d,%d)\n", MAJOR(dev->dev), MINOR(dev->dev));
  rc = vfs_mount("/dev", dev, get_filesystem("devfs"));
  if(IS_ERR(rc)) {
    printk("Could not mount devfs: %d\n", rc);
    printk("Continuing without a mounted devfs..\n");
  } else {
    printk("Successfully mounted devfs on block (%d,%d)\n", MAJOR(dev->dev), MINOR(dev->dev));
  }
  /* start init process */
  rc = start_init(cmdline);
  if(IS_ERR(rc))
    panic("Failed when starting init process: %d\n", rc);
  //printk("Nothing left to do. Going idle...\n");
  /* fall back to catk_idle (defined in proc/task.c:19)  */
}
