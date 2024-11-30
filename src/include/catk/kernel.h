#ifndef __KERNEL_H
#define __KERNEL_H

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
/* .mod_entries section */
extern uintptr_t kmod_start;
extern uintptr_t kmod_end;
/* .bss section */
extern uintptr_t kbss_start;
extern uintptr_t kbss_end;

void bootstrap2(void);
int start_init(const char * cmdline);

/* kernel stack size */
#define KERNEL_STACK_SIZE 4096 /* 4KiB of reserved memory */

#endif /* __KERNEL_H */
