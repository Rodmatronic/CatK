#ifndef __TRACE_H
#define __TRACE_H

#include <stdint.h>

struct kern_syms {
  uintptr_t addr;
  char * name;
};

void trace_stack(uint8_t frames);
char * trace_ret_addr(uintptr_t ret_eip);

#endif
