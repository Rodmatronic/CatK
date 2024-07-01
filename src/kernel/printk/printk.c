#include <stdarg.h>
#include <catk/printk.h>
#include <catk/compiler.h>
#include <catk/core.h>
#include <catk/spinlock.h>
#include <lib/common.h>

SPINLOCK_INIT(printk_spinlock);

int printk(const char format[], ...)
{
  int ret = 0;
  spinlock_acquire(&printk_spinlock);
  va_list arg;
  va_start(arg, format);
  ret = vprintf(format, arg);
  va_end(arg);
  spinlock_release(&printk_spinlock);
  return ret;
}
