#ifndef MEMORY_H
#define MEMORY_H
#include "types.h"
#include "../src/drivers/memory.c"

// I/O Ports for RTC
#define RTC_ADDRESS_PORT 0x70
#define RTC_DATA_PORT 0x71

void GetMemory();

#endif

