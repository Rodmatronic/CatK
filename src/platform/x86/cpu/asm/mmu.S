.section .text
  .global setup_pagetables

#define ASM_FILE 1

#include "../paging.h"

setup_pagetables:
  mov $kernel_pagetables, %eax
  orl $(1 << PTE_PRESENT_SHIFT | 1 << PTE_RW_SHIFT), %eax
  mov %eax, kernel_pagedir
  /* now for the page dir */
  xor %eax, %eax
  or $(1 << PDE_PRESENT_SHIFT | 1 << PDE_RW_SHIFT | 1 << PDE_PAGESIZE_SHIFT), %eax
  mov %eax, kernel_pagetables

  xor %eax, %eax
  mov %eax, (kernel_pagedir + 4)
  mov %eax, (kernel_pagetables + 4)
  /* load page directory */
  mov $kernel_pagedir, %eax
  mov %eax, %cr3

  mov %cr4, %eax
  or $(1 << 7 | 1 << 4), %eax
  mov %eax, %cr4

  mov %cr0, %eax
  or $(1 << 31), %eax
  mov %eax, %cr0

  ret

.section .data
.align PAGE_ALIGNMENT
kernel_pagetables: .space PAGE_SIZE
kernel_pagedir:    .space PAGE_SIZE
