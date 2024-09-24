#include <catk/printk.h>
#include <lib/common.h>
#include <stdarg.h>

void _module_print(const char * mod_name, const char * fmt, ...) {
  va_list args;
  va_start(args, fmt);
  printk("%s: ", mod_name);
  vprintf(fmt, args);
  va_end(args);
}
