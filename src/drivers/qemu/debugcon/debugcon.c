#include <catk/io.h>
#include <catk/kernel.h>
<<<<<<< HEAD
#include <catk/console.h>
=======
>>>>>>> 6be3bbb619dded66460b24ba64e5925e2bee774a
#include <lib/common.h>
#include <stdarg.h>
#include <stdint.h>
#include <config.h>

<<<<<<< HEAD
#define QEMU_DEBUGCON_PORT 0xE9
=======
#define QEMU_DEBUGCON_PORT 0xe9
>>>>>>> 6be3bbb619dded66460b24ba64e5925e2bee774a

static inline void qemu_debugcon_putc(uint8_t data) {
#ifdef CATK_DEBUG_BUILD
  outb(QEMU_DEBUGCON_PORT, data);
#endif
}

static inline void qemu_debugcon_puts(const char str[]) {
#ifdef CATK_DEBUG_BUILD
  for(int i = 0; str[i]; i++) {
    qemu_debugcon_putc((uint8_t)str[i]);
  }
#endif
}

void qemu_debugcon_printf(const char fmt[], ...) {
#ifdef CATK_DEBUG_BUILD
  char buffer[256];
  va_list arg;
  va_start(arg, fmt);
<<<<<<< HEAD
#ifndef CATK_SHOW_DEBUG
  vsnprintf(buffer, 255, fmt, arg); /* leave out a character for null byte */
  qemu_debugcon_puts(buffer);
#else
  if(is_console_enabled())
    vprintf(fmt, arg);
#endif
=======
  vsnprintf(buffer, 255, fmt, arg); /* leave out a character for null byte */
  qemu_debugcon_puts(buffer);
>>>>>>> 6be3bbb619dded66460b24ba64e5925e2bee774a
  va_end(arg);
#endif
}
