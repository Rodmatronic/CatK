#include <stdarg.h>
#include <catk/printk.h>
#include <catk/compiler.h>
#include <catk/core.h>
#include <catk/spinlock.h>
#include <lib/common.h>

SPINLOCK_INIT(printk_spinlock);

static void die()
{
  critical_enter();
  for(;;);
}

int printk(char * format, ...)
{
  spinlock_acquire(&printk_spinlock);
  va_list arg;
  va_start(arg, format);
  int ret = vprintf(format, arg);
  va_end(arg);
  spinlock_release(&printk_spinlock);
  return ret;
}

void panic(char * format, ...)
{
  va_list arg;
  va_start(arg, format);
  vprintf(strcat("CatK Panic: ", format), arg);
  va_end(arg);
  die();
  unreachable;
}
