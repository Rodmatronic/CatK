#ifndef __KERNEL_H
#define __KERNEL_H

#include <stdint.h>

char * obtain_cmdline(uint32_t addr);
void bootstrap2(void);
int start_init(void);

#endif
