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
#include <catk/debug.h>
#include <catk/task.h>
#include <lib/common.h>
#include <stdint.h>

/* page tables */
static uint32_t page_table[1024] _aligned(PAGE_ALIGNMENT);
static uint32_t heap_page_table[1024] _aligned(PAGE_ALIGNMENT);
static uint32_t framebuffer_page_table[1024] _aligned(PAGE_ALIGNMENT);
/* page directories */
static uint32_t kernel_page_dir[1024] _aligned(PAGE_ALIGNMENT);

static inline void tlb_flush(uint32_t v_addr) {
  asm volatile("invlpg (%0)" :: "r"(v_addr) : "memory");
}

uint32_t get_kernel_pgd(void) {
  return (uint32_t)kernel_page_dir;
}

static void paging_enable(void) {
  uint32_t cr0;
  asm volatile("mov %%cr0, %0" : "=r"(cr0));
  cr0 |= 0x80000001;
  asm volatile("mov %0, %%cr0" ::"r"(cr0));
}

static void paging_disable(void) {
  register uint32_t cr0;
  asm volatile("mov %%cr0, %0" : "=r"(cr0));
  cr0 &= ~0x80000000;
  asm volatile("mov %0, %%cr0" ::"r"(cr0));
}

static inline void load_page_directory(uint32_t pd)
{
  asm volatile("mov %0, %%cr3" :: "r"(pd));
}

static inline uint32_t get_fault_addr(void) {
  register uint32_t cr2;
  asm volatile("mov %%cr2, %0" : "=r"(cr2));
  return cr2;
}

static void page_fault_handler(struct intr_stack_frame * regs) {
  if(!is_console_enabled()) {
    debug("CRITICAL!! Page fault before console was enabled!!\n");
    debug("EIP: 0x%08x\n", regs->eip);
    critical_enter();
    halt();
  }
  struct task * task = get_current_task();
  printk("CatK bug report:\n");
  printk("------------[ cut here ]------------\n");
  printk("Task info:\n");
  printk("\tPID: %d (%s)\n", task->pid, task->name);
  printk("\tState: %d\n", task->state);
  printk("\tEnvironment: %s\n", task->kernel_mode ? "kernel" : "userland");
  printk("\tCR3 (Page directory): 0x%08x\n", task->cr3);
  printk("Important registers:\n");
  printk("\tEIP: 0x%08x, EFLAGS: 0x%08x, ESP: 0x%08x, EBP: 0x%08x\n", regs->eip, regs->eflags, regs->esp, regs->ebp);
  printk("\tCR2: (Fault address): 0x%08x\n", get_fault_addr());
  printk("----------[ end of dump ]-----------\n");
  if(task->kernel_mode) {
    panic("Page fault while in kernel mode\n");
  }
  kill(task);
}

/* creates page directory entry (pde) */
static inline uint32_t create_pde(uint32_t addr, uint8_t user, uint8_t rw) {
  return (uint32_t)((addr | 1 << PDE_PRESENT_SHIFT | rw << PDE_RW_SHIFT | user << PDE_USERMODE_SHIFT | 1 << PDE_PWT_SHIFT) & PDE_IGNORED); /* create pde, and clear all ignored bits */
}

/* creates page table entry (pte) */ 
static inline uint32_t create_pte(uint32_t addr, uint8_t user, uint8_t rw) {
  return (uint32_t)((addr | 1 << PTE_PRESENT_SHIFT | rw << PTE_RW_SHIFT | user << PTE_USERMODE_SHIFT | 1 << PTE_PWT_SHIFT) & PTE_IGNORED); /* create pte, and clear all ignored bits */
}

static uint32_t * copy_pte(uint32_t pde_entry) {
  uint32_t * new_pte = (uint32_t *)physmem_block_alloc();
  uint32_t * orig_pte = (uint32_t *)extract_pte(pde_entry);
  memcpy(new_pte, orig_pte, PAGE_SIZE);
  return new_pte;
}

static uint32_t * copy_pgd(uint32_t pgd_to_copy) {
  uint32_t * ret_pgd = (uint32_t *)physmem_block_alloc();
  memcpy(ret_pgd, &pgd_to_copy, PAGE_SIZE);
  for(int i = 0; i < 1024; i++) {
    if(ret_pgd[i]) {
      ret_pgd[i] = (uint32_t)copy_pte(ret_pgd[i]);
    }
  }
  return ret_pgd;
}

uint32_t * create_new_pgd(void) {
  uint32_t * pgd = copy_pgd(get_kernel_pgd());
  memset(pgd, 0, PAGE_SIZE);
  return pgd;
}

void paging_init(uint32_t addr)
{
  /* uefi systems might keep paging enabled */
  paging_disable();
  /* clear page stuff */
  memset(&kernel_page_dir, 0, PAGE_SIZE);
  memset(&page_table, 0, PAGE_SIZE);
  memset(&framebuffer_page_table, 0, PAGE_SIZE);
  /* initialize the physical memory manager for allocating page frames */
  physmem_init(addr);
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
  p_addr = 0xfd000000;
  size = 0x400000;
  for(int i = 0; size > 0; p_addr += PAGE_ALIGNMENT, size -= PAGE_ALIGNMENT, i++) {
    framebuffer_page_table[i] = create_pte(p_addr, 0, 1);
  }
  kernel_page_dir[0] = create_pde((uint32_t)page_table, 0, 1);
  kernel_page_dir[1] = create_pde((uint32_t)heap_page_table, 0, 1);
  kernel_page_dir[2] = create_pde((uint32_t)framebuffer_page_table, 0, 1);
  load_page_directory((uint32_t)kernel_page_dir);
  paging_enable();
  interrupt_install(page_fault_handler, 0x0e);
}

static void virtmem_map(uint32_t page_dir, uint32_t phys_addr, uint32_t virt_addr, uint8_t user, uint8_t rw) {
  uint32_t * pde = (uint32_t *)page_dir;
  uint32_t pt_index = pte_index(virt_addr);
  uint32_t pd_index = pde_index(virt_addr);
  
  assert(pt_index <= 1024);
  assert(pd_index <= 1024);

  if(pde_is_present(pde[pd_index]) == false) {
    uint32_t page_frame_addr = physmem_block_alloc();
    memset((void *)page_frame_addr, 0, PAGE_SIZE);
    pde[pd_index] = create_pde(page_frame_addr, user, rw);
    tlb_flush(virt_addr);
  }
  uint32_t * pte = (uint32_t *)extract_pte(pde[pd_index]);
  pte[pt_index] = create_pte(phys_addr, user, rw);
  tlb_flush(virt_addr);
}

/* 
* kvm = kernel virtual memory 
* uvm = user virtual memory
*/

/* the kernel usually uses this for itself :P */
void kvm_map(uint32_t phys_addr, uint32_t virt_addr) {
  virtmem_map(get_kernel_pgd(), phys_addr, virt_addr, 0, 1);
}

/* the kernel will always use this for a user-mode process (like for sys_mmap, sys_munmap, or task creation) */
void uvm_map(uint32_t pgd, uint32_t phys_addr, uint32_t virt_addr) {
  virtmem_map(pgd, phys_addr, virt_addr, 1, 1);
}
