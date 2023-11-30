#ifndef CPU_H
#define CPU_H

#include "types.h"
#include "../src/drivers/cpu.c"

uint32 brand[12];

void cpuid_info(int print);

#endif

