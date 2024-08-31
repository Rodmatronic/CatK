#ifndef __KERNEL_H
#define __KERNEL_H

/* kernel stack size */
#define KERNEL_STACK_SIZE 4096 /* 4KiB of reserved memory */

#ifndef ASM_FILE

#include <stdint.h>
#include <lib/common.h>

extern uintptr_t kstart;
extern uintptr_t kend;
/* .text section */
extern uintptr_t ktext_start;
extern uintptr_t ktext_end;
/* .rodata section */
extern uintptr_t krodata_start;
extern uintptr_t krodata_end;
/* .data section */
extern uintptr_t kdata_start;
extern uintptr_t kdata_end;
/* .bss section */
extern uintptr_t kbss_start;
extern uintptr_t kbss_end;

extern bool kern_verbose;

char * obtain_cmdline(uint32_t addr);
void bootstrap2(void);
int start_init(const char * cmdline);

#endif /* ASM_FILE */

#endif /* __KERNEL_H */
