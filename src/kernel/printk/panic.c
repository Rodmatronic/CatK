#include <stdarg.h>
#include <catk/printk.h>
#include <catk/compiler.h>
#include <catk/platform.h>
#include <catk/trace.h>
#include <catk/debug.h>
#include <catk/kernel.h>
#include <catk/mem.h>
#include <lib/common.h>

static void _cold_ die()
{
  critical_enter();
  halt();
}

void _cold_ _noreturn_ panic(const char format[], ...)
{
  va_list arg;
  va_start(arg, format);
  vprintf(strcat("Panic!: ", format), arg); // combine both strings to make one
  va_end(arg);
  die();
  unreachable;
}
