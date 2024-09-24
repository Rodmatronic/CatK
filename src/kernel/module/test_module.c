#include <catk/module.h>
#include <catk/printk.h>
#include <catk/errno.h>

MODULE_NAME("test_module");
MODULE_AUTHOR("deyzi-the-youtuber")

static int testmod_init(void) {
  module_print("This is a built-in kernel module in CatK! :)\n");
  return 0;
}

MODULE_START
  .init = testmod_init
MODULE_END
