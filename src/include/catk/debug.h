#ifndef __DEBUG_H
#define __DEBUG_H

#include <catk/printk.h>
#include <lib/common.h>
#include <config.h>

#define assert(condition) \
  if(!(condition)) {\
    panic("ASSERT FAILED at (%s:%d): %s\n", __FILE__, __LINE__, #condition); \
  }

#define debug(...) \
  qemu_debugcon_printf("[" __FILE__ "]: " __VA_ARGS__)

void qemu_debugcon_printf(const char fmt[], ...);

#endif
