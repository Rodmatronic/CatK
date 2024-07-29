#include <stdarg.h>
#include <catk/printk.h>
#include <catk/compiler.h>
#include <catk/core.h>
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

void _cold_ panic(const char format[], ...)
{
  beep(50);
  msleep(50);
  beep(50);
  msleep(50);
  beep(50);
  trace_stack(8);
  va_list arg;
  va_start(arg, format);
  vprintf(strcat("Panic!: ", format), arg); // combine both strings to make one
  va_end(arg);
  die();
  unreachable;
}
