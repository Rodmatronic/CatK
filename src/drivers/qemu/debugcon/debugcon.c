#include <catk/io.h>
#include <catk/kernel.h>
#include <catk/console.h>
#include <lib/common.h>
#include <stdarg.h>
#include <stdint.h>
#include <config.h>

#pragma GCC diagnostic ignored "-Wunused-function"

#define QEMU_DEBUGCON_PORT 0xE9

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
#ifndef CATK_SHOW_DEBUG
  vsnprintf(buffer, 255, fmt, arg); /* leave out a character for null byte */
  qemu_debugcon_puts(buffer);
#else
  if(is_console_enabled())
    vprintf(fmt, arg);
#endif
  va_end(arg);
#endif
}
