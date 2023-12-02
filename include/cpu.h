#ifndef CPU_H
#define CPU_H

#include "types.h"

extern uint32 brand[12];

void cpuid_info(int print);
void cpusimple(int print);

#endif

