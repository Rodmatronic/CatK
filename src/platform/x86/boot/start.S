.section .text
  .global kentry
  .extern kmain
  .extern printk
  .extern kbss_start
  .extern kbss_end

#define ASM_FILE 1

#include <catk/kernel.h>

kentry:
  /* initialize the stack pointer. */
  movl $(kstack + KERNEL_STACK_SIZE), %esp
  /* this will be useful later for stack tracing */
  xorl %ebp, %ebp

  /* reset EFLAGS. */
  pushl $0
  popfl

  pushl %ebx
  pushl %eax

  call kmain
        
loop:
  jmp loop

.section .bss
  .comm kstack, KERNEL_STACK_SIZE