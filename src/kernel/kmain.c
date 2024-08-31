#include <stdint.h>
#include <multiboot2.h>
#include <catk/errno.h>
#include <catk/console.h>
#include <catk/printk.h>
#include <catk/platform.h>
#include <catk/mem.h>
#include <catk/kernel.h>

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
  printk("\n\033[1;36m          /\\      /\\\n");
  printk("         //\\\\    //\\\\\n");
  printk("        //  \\\\__//  \\\\\n");
  printk("       //`          `\\\\\n");
  printk("      /     |    |     \\\n");
  printk("    \\|      |    |      |/\n");
  printk("   --|   //    ^     // |--\n");
  printk("    / \\      \\/\\/      / \\\n");
  printk("       \",            ,\"\n");
  printk("\033[1;31m         `/========\\`\n");
  printk("\033[1;31m         /====\033[33m/\\\033[31m====\\\n");
  printk("\033[33m            /'  '\\*\n");
  printk("           | CatK |\n");
  printk("           *\\,__,/\033[1;0m\n\n");
}

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
  console_puts("\033[1;31mC\033[32mO\033[33mL\033[34mO\033[35mR\033[1;0m video console initialized :)\n");
  physmem_init(mbi);
  early_platform_init();
  printk("PHYSICAL MEMORY ALLOCATION TEST!\n");
  void * spungbub = physmem_alloc_block();
  printk("RESULT IS %s: 0x%08x\n", (spungbub == NULL) ? "FAIL" : "PASS", spungbub);
  for(;;);
  show_boot_banner();
  printk("Early platform has been initialized\n");
  draw_logo();
}
