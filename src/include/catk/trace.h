#ifndef __TRACE_H
#define __TRACE_H

#include <stdint.h>

struct kern_syms
{
  uint32_t addr;
  char * name;
};

void trace_stack(int frames);

#endif