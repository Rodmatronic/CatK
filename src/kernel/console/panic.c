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

static void _cold_ die()
{
  critical_enter();
  halt();
}

void _cold_ _noreturn_ panic(const char format[], ...)
{
  va_list arg;
  va_start(arg, format);
  printk("Panic!: ");
  printk(format, arg);
  uint8_t random = rand() % 3;
  if(random > 2) {
    random = 1; // get over this stupid bug
  }
  debug("Hmmm... Im gonna pick number %d!\n", random);
  printk("%s", messages[random]);
  va_end(arg);
  die();
  unreachable;
}
