#include <stdarg.h>
#include <catk/printk.h>
#include <catk/compiler.h>
#include <catk/core.h>
#include <catk/trace.h>
#include <catk/debug.h>
#include <catk/kernel.h>
#include <catk/mem.h>
#include <catk/task.h>
#include <lib/common.h>

/* triggers a kernel oops */
void _cold_ oops(const char format[], ...)
{
  beep(50);
  msleep(50);
  beep(50);
  trace_stack(8);
  va_list arg;
  va_start(arg, format);
  vprintf(strcat("Oops!: ", format), arg); // combine both strings to make one
  va_end(arg);
  kill(get_current_task());
  unreachable;
}