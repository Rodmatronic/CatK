#include <stdarg.h>
#include <catk/printk.h>
#include <catk/compiler.h>
#include <catk/spinlock.h>
#include <lib/common.h>

SPINLOCK_INIT(print_lock);

int printk(const char format[], ...)
{
  spinlock_acquire(&print_lock);
  int ret = 0;
  va_list arg;
  va_start(arg, format);
  ret = vprintf(format, arg);
  va_end(arg);
  spinlock_release(&print_lock);
  return ret;
}
