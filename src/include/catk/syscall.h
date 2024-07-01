#ifndef __SYSCALL_H
#define __SYSCALL_H

#include <stdint.h>
#include <catk/compiler.h>

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
  uint16_t ds;
  uint16_t es;
  uint16_t fs;
  uint16_t gs;
  uint32_t orig_eax;
  uint32_t eip;
  uint32_t cs;
  uint32_t eflags;
  uint32_t esp;
  uint32_t ss;
}_packed_;

void sys_exit(int err_code);

#endif
