#ifndef __KERNEL_H
#define __KERNEL_H

#include <stdint.h>
#include <lib/common.h>

extern bool kern_verbose;

char * obtain_cmdline(uint32_t addr);
void bootstrap2(void);
int start_init(const char * cmdline);

#endif
