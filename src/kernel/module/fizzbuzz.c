#include <catk/module.h>
#include <catk/printk.h>

MODULE_NAME("Test Module");
MODULE_AUTHOR("deyzi-the-youtuber");

// Custom init and exit methods

static int custom_init(void) {
  printk("This is a kernel module! :D\n");
  return 0;
}

static void custom_exit(void) {
  printk("Goodbye my friend, I shall miss you dearly...\n");
}

module_init(custom_init, custom_exit);
