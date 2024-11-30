#include <catk/module.h>
#include <catk/printk.h>
#include <catk/debug.h>
#include <catk/kernel.h>
#include <catk/errno.h>

static struct module * module_list[32];
static int modules_loaded = 0;

void module_unload(const char * mod_name) {
  for(int i = 0; i < 32; i++) {
    if(strncmp(module_list[i]->name, mod_name, strlen(module_list[i]->name)) == 0) {
      module_list[i]->exit();
    }
  }
}

void module_load(const char * mod_name) {
  for(int i = 0; i < 32; i++) {
    if(strncmp(module_list[i]->name, mod_name, strlen(module_list[i]->name)) == 0) {
      module_list[i]->init();
    }
  }
}

static void modules_load(void) {
  for(int i = 0; i < modules_loaded; i++) {
    struct module * mod = module_list[i];
    debug("Initializing module %s created by \"%s\"..\n", mod->name, mod->author);
    int rc = mod->init();
    if(IS_ERR(rc)) {
      debug("Module %s failed to initialize: %d\n", mod->name, rc);
    }
  }
}

void builtin_modules_load(void) {
  memset(&module_list, 0, sizeof(struct module *) * 32); // would equate to 128 bytes
  if(NO_LOADED_MODULES) {
    printk("No kernel modules to load.\n");
    return;
  }
  debug("Loading all built-in kernel modules...\n");
  debug("Built-in kernel modules: [0x%08x - 0x%08x]\n", &kmod_start, &kmod_end);
  for(struct module * mod = (struct module *)&kmod_start; mod != (struct module *)&kmod_end; mod++) {

    module_list[modules_loaded] = mod;
    modules_loaded++;
  }
  printk("Loaded %d kernel module(s).\n", modules_loaded);
  modules_load();
}
