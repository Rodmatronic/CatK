#include <stdint.h>
#include <multiboot2.h>
#include <catk/errno.h>
#include <catk/console.h>
#include <catk/printk.h>
#include <catk/platform.h>
#include <catk/mem.h>
#include <catk/kernel.h>
#include <catk/keyb.h>
#include <catk/version.h>
#include <catk/utsname.h>
#include <config.h>

#ifndef __GNUC__
#error "Compile with GCC or Clang please! :)"
#endif

static void show_boot_banner(void)
{
  printk("2023-2024 The CatKernel Project.\n");
  printk("\tCreated locally in Canada, and California, bring tuques and cold drinks.\n");
  printk("\nThis software is licensed under the GNU General Public License v3.0.\n");
  printk("Everyone is permitted to copy, distribute, and modify this software.\n\n");
}

static inline void draw_logo(void) {
  printk("\n\033[1;37m          /\\      /\\            \033[1;37m_____       _______ _  __\n");
  printk("\033[36m         //\\\\    //\\\\          \033[1;37m/ ____|   /\\|__   __| |/ /\n");
  printk("\033[36m        //  \\\\__//  \\\\        \033[36m| |       /  \\  | |  | ' / \n");
  printk("\033[36m       //`          `\\\\       \033[36m| |      / /\\ \\ | |  |  <  \n");
  printk("\033[36m      /     |    |     \\      \033[36m| |____ / ____ \\| |  | . \\ \n");
  printk("\033[36m    \\|      |    |      |/     \033[1;36m\\_____/_/    \\_\\_|  |_|\\_\\ \n");
  printk("\033[1;36m   --|   //    ^     // |--   \033[1;37m== The Kitty Unix kernel ==\n");
  printk("\033[1;36m    / \\      \\/\\/      / \\    Brought to you by:\n");
  printk("\033[36m       \",            ,\"        \033[1;36m- Rodmatronics\n");
  printk("\033[1;36m         `\033[31m/========\\\033[36m`          \033[1;36m- Virus_large\n");
  printk("\033[31m         /====\033[1;33m/\\\033[1;31m====\\          \033[36m- Vera (irix_aligned)\n");
  printk("\033[1;33m            /'  '\\*\n");
  printk("           | CatK |\n");
  printk("           *\\,__,/\033[1;0m\n\n");
}

extern int keyboard_init(void);

void kmain(uint32_t magic, uintptr_t mbi) {
  int rc = multiboot2_validate(magic, mbi);
  if(rc < 1) {
    return;
  }
  /* enable console for early debugging */
  rc = console_init(mbi);
  if(IS_ERR(rc)) {
    return;
  }
  printk(catk_boot_banner, UTS_RELEASE, CATK_VERSION_STRING, CATK_COMPILED_WITH, CATK_BUILD_DATE);
  console_puts("\033[1;31mC\033[32mO\033[33mL\033[34mO\033[35mR\033[1;0m video console initialized :)\n");
  console_puts("Initializing early platform..\n");
  physmem_init(mbi);
  early_platform_init();
  /* 
  * the console should be disabled in src/platform/x86/boot/mmu.c since it isn't mapped into virtual memory.
  * we'll map it in a jiffy :)
  */
  console_map_virt();
  /*
  * now we can reenable the console now :)
  */
  //console_enable();
  keyb_init();
  show_boot_banner();
  printk("Early platform has been initialized\n");
  draw_logo();
}
