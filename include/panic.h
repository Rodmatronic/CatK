#ifndef PANIC_H
#define PANIC_H

#include "types.h"
#include "../src/drivers/panic.c"

void panic(const char* panictext, ...);

#endif

