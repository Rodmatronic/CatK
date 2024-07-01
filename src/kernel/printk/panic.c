#include <stdarg.h>
#include <catk/printk.h>
#include <catk/compiler.h>
#include <catk/core.h>
#include <lib/common.h>

static void die()
{
  critical_enter();
  for(;;);
}

void panic(const char format[], ...)
{
  va_list arg;
  va_start(arg, format);
  vprintf(strcat("Panic!: ", format), arg); // combine both strings to make one
  va_end(arg);
  die();
  unreachable;
}
