#include <catk/module.h>
#include <catk/printk.h>

MODULE_NAME("test_mod");
MODULE_AUTHOR("deyzi-the-youtuber");

// Custom init and exit methods

static int custom_init(void) {
  module_print("Kernel module INIT\n");
  return 0;
}

static void custom_exit(void) {
  module_print("Kernel module EXIT\n");
}

module_init(custom_init, custom_exit);
