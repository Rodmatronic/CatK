#include <stdarg.h>
#include <catk/printk.h>
#include <catk/compiler.h>
#include <catk/core.h>
#include <catk/trace.h>
#include <catk/debug.h>
#include <catk/kernel.h>
#include <catk/mem.h>
#include <catk/console.h>
#include <lib/common.h>

static const char messages[4][128] = {
  "The kernel is now in limbo. Kitty limbo!",
  "CatKernel has spontaneously combusted, and has been halted to prevent further damage.",
  "CatKernel is now trapped in the kitty void, please reboot."
};

void _cold_ _noreturn_ panic(const char format[], ...)
{
#ifdef CATK_STACK_TRACE
  trace_stack(8);
#endif
  va_list arg;
  va_start(arg, format);
#ifdef CATK_DEBUG_BUILD
  printk("Panic at %s()!: ", trace_ret_addr((uintptr_t)__builtin_extract_return_addr(__builtin_return_address (0))));
#else
  printk("Panic!: ");
#endif
  vprintf(format, arg);
  uint8_t random = rand() % 3;
  if(random > 2) {
    random = 1; // get over this stupid bug
  }
  printk("%s", messages[random]);
  va_end(arg);
  die();
  unreachable;
}
