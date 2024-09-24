#include <stdarg.h>
#include <catk/printk.h>
#include <catk/compiler.h>
#include <catk/platform.h>
#include <catk/trace.h>
#include <catk/debug.h>
#include <catk/kernel.h>
#include <catk/mem.h>
#include <catk/console.h>
#include <lib/common.h>

<<<<<<< HEAD
static const char messages[4][128] = {
  "The kernel is now in limbo. Kitty limbo!",
  "CatKernel has spontaneously combusted, and has been halted to prevent further damage.",
  "CatKernel is now trapped in the kitty void, please reboot."
};

=======
>>>>>>> 6be3bbb619dded66460b24ba64e5925e2bee774a
static void _cold_ die()
{
  critical_enter();
  halt();
}

void _cold_ _noreturn_ panic(const char format[], ...)
{
  va_list arg;
  va_start(arg, format);
<<<<<<< HEAD
  printk("Panic!: ");
  printk(format, arg);
  uint8_t random = rand() % 3;
  if(random > 2) {
    random = 1; // get over this stupid bug
  }
  debug("Hmmm... Im gonna pick number %d!\n", random);
  printk("%s", messages[random]);
=======
  vprintf(strcat("Panic!: ", format), arg); // combine both strings to make one
>>>>>>> 6be3bbb619dded66460b24ba64e5925e2bee774a
  va_end(arg);
  die();
  unreachable;
}
