#ifndef __SYSCALL_H
#define __SYSCALL_H

#include <stdint.h>

/* 
* borrowed from linux source code 
* this is ONLY for syscalls.
*/
struct pt_regs
{
  uint32_t ebx;
  uint32_t ecx;
  uint32_t edx;
  uint32_t esi;
  uint32_t edi;
  uint32_t ebp;
  uint32_t eax;
  uint32_t orig_eax;
  uint32_t eip;
  uint16_t cs;
  uint32_t eflags;
  uint32_t esp;
  uint16_t ss;
};

void sys_exit(int err_code);

#endif
