#include <catk/module.h>
#include <catk/printk.h>
#include <catk/debug.h>
#include <catk/kernel.h>
#include <catk/errno.h>

void builtin_modules_init(void) {
  int rc;
  debug("Loading built-in kernel modules...\n");
  printk("Built-in kernel modules: [0x%08x - 0x%08x]\n", &kmod_start, &kmod_end);
  for(struct module * mod = (struct module *)&kmod_start; mod != (struct module *)&kmod_end; mod++) {
    debug("Calling module \"%s\" created by %s..\n", mod->name, mod->author);
    rc = mod->init();
    if(IS_ERR(rc)) {
      printk("Module \"%s\" failed to initialize: %d\n", mod->name, rc);
    }
  }
}
