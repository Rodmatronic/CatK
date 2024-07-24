#ifndef __DEBUG_H
#define __DEBUG_H

#include <stdarg.h>
#include <lib/common.h>

void serial_init(void);
void debug(const char * fmt, ...);

#endif
