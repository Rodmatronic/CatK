#ifndef __DEBUG_H
#define __DEBUG_H

#include <catk/printk.h>
#include <stdarg.h>
#include <lib/common.h>

#define assert(condition) \
  if(!(condition)) {\
    panic("ASSERT FAILED at (%s:%d): %s\n", __FILE__, __LINE__, #condition); \
  }

void serial_init(void);
void debug(const char * fmt, ...);

#endif
