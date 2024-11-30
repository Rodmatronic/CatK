/*

  catk/src/mem/paging.c

  The CatK Project 2023 - 2024
  Author: Foo Bar

  Description:
    Specifically for x86 / x86_64 systems. Sets up page tables and provides useful tools.

*/

#include <catk/virt.h>
#include <catk/debug.h>
#include <catk/compiler.h>
#include <catk/mem.h>
#include <catk/core.h>
#include <catk/console.h>
#include <catk/kernel.h>
#include <catk/task.h>
#include <catk/ipc.h>
#include <multiboot2.h>
#include <lib/common.h>
#include <stdint.h>
#include <config.h>

/* page tables */
static uint32_t page_table[1024] _aligned(PAGE_ALIGNMENT);
static uint32_t heap_page_table[1024] _aligned(PAGE_ALIGNMENT);
static uint32_t heap_page_table2[1024] _aligned(PAGE_ALIGNMENT);
#ifndef CATK_VIDEO_GENERIC
//static uint32_t framebuffer_page_table[1024] _aligned(PAGE_ALIGNMENT);
#endif
/* page directories */
static uint32_t kernel_page_dir[1024] _aligned(PAGE_ALIGNMENT);

static inline void tlb_flush(uint32_t v_addr) {
  asm volatile("invlpg (%0)" :: "r"(v_addr) : "memory");
}

uint32_t get_kernel_pgd(void) {
  return (uint32_t)kernel_page_dir;
}

static void paging_enable(void) {
  register uint32_t cr0;
  asm volatile("mov %%cr0, %0" : "=r"(cr0));
  cr0 |= 0x80000000;
  asm volatile("mov %0, %%cr0" ::"r"(cr0));
}

static void paging_disable(void) {
  register uint32_t cr0;
  asm volatile("mov %%cr0, %0" : "=r"(cr0));
  cr0 &= ~0x80000000;
  asm volatile("mov %0, %%cr0" ::"r"(cr0));
}

static inline uint32_t get_fault_addr(void) {
  register uint32_t cr2;
  asm volatile("mov %%cr2, %0" : "=r"(cr2));
  return cr2;
}

static void page_fault_handler(struct intr_stack_frame * regs) {
  critical_enter();
  if(is_console_enabled() == false) {
    die();
    unreachable;
  }
  struct task * task = NULL;
  bool user_mode = false;
  if(is_tasking_enabled() == true) {
    task = get_current_task();
    user_mode = (task->regs.cs == 0x08);
  }
  printk("CatK bug report:\n");
  printk("------------[ cut here ]------------\n");
  printk("Important registers:\n");
  printk("\tEIP: 0x%08x, EFLAGS: 0x%08x, ESP: 0x%08x, EBP: 0x%08x\n", regs->eip, regs->eflags, regs->esp, regs->ebp);
  printk("\tCR2: (Fault address): 0x%08x, CR3: (Page directory): 0x%08x\n", get_fault_addr(), task->pgd);
  printk("Causes:\n");
  printk("\t%s, %s, and the fault %s\n", 
          regs->err_code & BIT(0) ? "Page protection fault" : "Page not present",
          regs->err_code & BIT(1) ? "write access" : "read access",
          regs->err_code & BIT(2) ? "occured in user mode" : "occured in kernel mode"
  );
  printk("Error code: 0x%08x\n", regs->err_code);
  printk("----------[ end of dump ]-----------\n");
  critical_exit();
  if(user_mode == false) {
    panic("Page fault while in kernel mode\n");
  } else {
    dispatch_signal(SIGSEGV);
  }
  if(is_tasking_enabled() == true) {
    kill(task);
  } else {
    die();
    unreachable;
  }
}

/* creates page directory entry (pde) */
static inline uint32_t create_pde(uint32_t addr, uint8_t user, uint8_t rw) {
  return (uint32_t)((addr | 1 << PDE_PRESENT_SHIFT | rw << PDE_RW_SHIFT | user << PDE_USERMODE_SHIFT | 1 << PDE_PWT_SHIFT) & PDE_IGNORED); /* create pde, and clear all ignored bits */
}

/* creates page table entry (pte) */ 
static inline uint32_t create_pte(uint32_t addr, uint8_t user, uint8_t rw) {
  return (uint32_t)((addr | 1 << PTE_PRESENT_SHIFT | rw << PTE_RW_SHIFT | user << PTE_USERMODE_SHIFT | 1 << PTE_PWT_SHIFT) & PTE_IGNORED); /* create pte, and clear all ignored bits */
}

uint32_t create_new_pgd(void) {
  uint32_t * pgd = physmem_alloc_block();
  debug("Created new page-directory @ 0x%08x\n", pgd);
  assert(pgd != NULL);
  return (uint32_t)pgd;
}

void paging_init(void)
{
  /* uefi systems might keep paging enabled */
  paging_disable();
  /* clear page stuff */
  memset(&kernel_page_dir, 0, PAGE_SIZE);
  memset(&page_table, 0, PAGE_SIZE);
  memset(&heap_page_table, 0, PAGE_SIZE);
  memset(&heap_page_table2, 0, PAGE_SIZE);
  /*
#ifndef CATK_VIDEO_GENERIC
  memset(&framebuffer_page_table, 0, PAGE_SIZE);
#endif
*/
  /* do identity paging */
  for(int i = 0; i < 1024; i++) {
    kernel_page_dir[i] = create_pde(0x00000000, 0, 1) & ~(1 << PDE_PRESENT_SHIFT); /* not present */
  }
  uint32_t p_addr = 0x00000000, size = 0x400000;
  for(int i = 0; size > 0; p_addr += PAGE_ALIGNMENT, size -= PAGE_ALIGNMENT, i++) {
    page_table[i] = create_pte(p_addr, 0, 1);
  }
  size = 0x400000;
  for(int i = 0; size > 0; p_addr += PAGE_ALIGNMENT, size -= PAGE_ALIGNMENT, i++) {
    heap_page_table[i] = create_pte(p_addr, 0, 1);
  }
  size = 0x400000;
  for(int i = 0; size > 0; p_addr += PAGE_ALIGNMENT, size -= PAGE_ALIGNMENT, i++) {
    heap_page_table2[i] = create_pte(p_addr, 0, 1);
  }
/*
#ifndef CATK_VIDEO_GENERIC
  p_addr = ((struct multiboot_tag_framebuffer_common *)multiboot2_locate_tag(multiboot2_get_mbi(), MULTIBOOT_TAG_TYPE_FRAMEBUFFER))->framebuffer_addr;
  size = 0x400000;
  for(int i = 0; size > 0; p_addr += PAGE_ALIGNMENT, size -= PAGE_ALIGNMENT, i++) {
    framebuffer_page_table[i] = create_pte(p_addr, 0, 1);
  }
#endif
*/
  page_table[0] = 0;
  kernel_page_dir[0] = create_pde((uint32_t)page_table, 0, 1);
  kernel_page_dir[1] = create_pde((uint32_t)heap_page_table, 0, 1);
  kernel_page_dir[2] = create_pde((uint32_t)heap_page_table2, 0, 1);
/*
#ifndef CATK_VIDEO_GENERIC
  kernel_page_dir[3] = create_pde((uint32_t)framebuffer_page_table, 0, 1);
#endif
*/
  load_page_directory((uint32_t)kernel_page_dir);
  paging_enable();
  interrupt_install(page_fault_handler, 0x0e);
}

static void _hot_ virtmem_map(uint32_t page_dir, uint32_t phys_addr, uint32_t virt_addr, uint8_t user, uint8_t rw) {
  if((phys_addr % 4096) != 0) {
    debug("Uh oh, 0x%08x isn't page aligned!\n", phys_addr);
    return;
  }
  uint32_t * pde = (uint32_t *)page_dir;
  uint16_t pt_index = pte_index(virt_addr);
  uint16_t pd_index = pde_index(virt_addr);
  /* make sure it isnt out of bounds */
  if(pt_index > 1024 || pd_index > 1024) {
    return;
  }
  if(pde_is_present(pde[pd_index]) == false) {
    uint32_t page_frame = (uint32_t)physmem_alloc_block();
    assert(page_frame != 0);
    memset((void *)page_frame, 0, PAGE_SIZE);
    pde[pd_index] = create_pde(page_frame, user, rw);
    tlb_flush(virt_addr);
  }
  uint32_t * pte = (uint32_t *)extract_pte(pde[pd_index]);
  pte[pt_index] = create_pte(phys_addr, user, rw);
  tlb_flush(virt_addr);
}

static void virtmem_unmap(uint32_t page_dir, uint32_t virt_addr) {
  uint32_t * pde = (uint32_t *)page_dir;
  uint32_t pt_index = pte_index(virt_addr);
  uint32_t pd_index = pde_index(virt_addr);
  /* make sure it isnt out of bounds */
  if(pt_index > 1024 || pd_index > 1024) {
    return;
  }
  
  if(pde_is_present(pde[pd_index]) == true) {
    pde[pd_index] &= ~(1 << PDE_PRESENT_SHIFT);
    tlb_flush(virt_addr);
  }
  uint32_t * pte = (uint32_t *)extract_pte(pde[pd_index]);
  pte[pt_index] &= ~(1 << PTE_PRESENT_SHIFT);
  tlb_flush(virt_addr);
}

/* 
* kvm = kernel virtual memory 
* uvm = user virtual memory
*/

/* the kernel usually uses this for itself :P */
void _hot_ kvm_map(uint32_t phys_addr, uint32_t virt_addr) {
  virtmem_map(get_kernel_pgd(), phys_addr, virt_addr, 0, 1);
}

void kvm_unmap(uint32_t virt_addr) {
  virtmem_unmap(get_kernel_pgd(), virt_addr);
}

/* the kernel will always use this for a user-mode process (like for sys_mmap, sys_munmap, or task creation) */
void _hot_ uvm_map(uint32_t pgd, uint32_t phys_addr, uint32_t virt_addr, uint8_t rw) {
  virtmem_map(pgd, phys_addr, virt_addr, 1, rw);
}
