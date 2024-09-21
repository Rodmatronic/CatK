#include <stdarg.h>
#include <catk/printk.h>
#include <catk/compiler.h>
#include <lib/common.h>

int printk(const char format[], ...)
{
  int ret = 0;
  va_list arg;
  va_start(arg, format);
  ret = vprintf(format, arg);
  va_end(arg);
  return ret;
}
