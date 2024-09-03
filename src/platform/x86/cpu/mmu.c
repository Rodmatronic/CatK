#include <catk/debug.h>
#include <catk/printk.h>
#include <catk/compiler.h>
#include <catk/mem.h>
#include <catk/platform.h>
#include <catk/console.h>
#include <catk/io.h>
#include <lib/common.h>
#include <stdint.h>

#include "irq.h"
#include "paging.h"

/* page directories */
static uint32_t kernel_page_dir[1024] _aligned(PAGE_ALIGNMENT);
static uint32_t kernel_page_table[1024] _aligned(PAGE_ALIGNMENT);
static uint32_t pmm_page_table[1024] _aligned(PAGE_ALIGNMENT);

static inline void tlb_flush(uint32_t v_addr) {
  asm volatile("invlpg (%0)" :: "r"(v_addr) : "memory");
}

static inline uint32_t get_fault_addr(void) {
  register uint32_t cr2;
  asm volatile("mov %%cr2, %0" : "=r"(cr2));
  return cr2;
}

static void page_fault_handler(struct intr_stack_frame * regs) {
  printk("CatK paging bug report:\n");
  printk("------------[ cut here ]------------\n");
  printk("Important registers:\n");
  printk("\tEIP: 0x%08x, EFLAGS: 0x%08x, ESP: 0x%08x, EBP: 0x%08x\n", regs->eip, regs->eflags, regs->esp, regs->ebp);
  printk("\tCR2: (Fault address): 0x%08x\n", get_fault_addr());
  printk("Causes:\n");
  printk("\t%s, %s, and the fault %s\n", 
          regs->error & BIT(0) ? "Page protection fault" : "Page not present",
          regs->error & BIT(1) ? "write access" : "read access",
          regs->error & BIT(2) ? "occured in user mode" : "occured in kernel mode"
  );
  printk("Error code: 0x%08x\n", regs->error);
  printk("----------[ end of dump ]-----------\n");
  critical_enter();
  halt();
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
  uint32_t * new_pte = (uint32_t *)physmem_alloc_block();
  uint32_t * orig_pte = (uint32_t *)extract_pte(pde_entry);
  memcpy(new_pte, orig_pte, PAGE_SIZE);
  return new_pte;
}

static uint32_t * copy_pgd(uint32_t pgd_to_copy) {
  uint32_t * ret_pgd = (uint32_t *)physmem_alloc_block();
  memcpy(ret_pgd, &pgd_to_copy, PAGE_SIZE);
  for(int i = 0; i < 1024; i++) {
    if(ret_pgd[i]) {
      ret_pgd[i] = (uint32_t)copy_pte(ret_pgd[i]);
    }
  }
  return ret_pgd;
}

uint32_t * create_new_pgd(void) {
  uint32_t * pgd = copy_pgd((uint32_t)kernel_page_dir);
  memset(pgd, 0, PAGE_SIZE);
  return pgd;
}

static void paging_identity_map(uint32_t phys_start, uint32_t virt_start) {
  uint32_t * page_table = (uint32_t *)physmem_alloc_block();
  if(!page_table) {
    panic("Ran out of memory while allocating page table!");
  }
  for(int i = 0; i < 1024; i++) {
    page_table[i] = create_pte(phys_start, 0, 1);
    phys_start += PAGE_SIZE;
  }
  kernel_page_dir[pde_index(virt_start)] = create_pde((uint32_t)page_table, 0, 1);
}

void setup_paging(void)
{
  /* do identity paging */
  paging_identity_map(0x0, 0x0);
  paging_identity_map(0x400000, 0x400000);
  paging_identity_map(0x800000, 0x800000);
  native_load_pagedir((uint32_t)kernel_page_dir);
  native_enable_paging();
  console_disable();
  intr_add_handler(X86_TRAP_PF, page_fault_handler);
}

static void native_map_virtual(uint32_t page_dir, uint32_t phys_addr, uint32_t virt_addr, uint8_t user, uint8_t rw) {
  uint32_t * pde = (uint32_t *)page_dir;
  uint32_t pt_index = pte_index(virt_addr);
  uint32_t pd_index = pde_index(virt_addr);
  if(pde_is_present(pde[pd_index]) == false) {
    uint32_t page_frame_addr = (uint32_t)physmem_alloc_block();
    memset((void *)page_frame_addr, 0, PAGE_SIZE);
    pde[pd_index] = create_pde(page_frame_addr, user, rw);
    tlb_flush(virt_addr);
  }
  uint32_t * pte = (uint32_t *)extract_pte(pde[pd_index]);
  pte[pt_index] = create_pte(phys_addr, user, rw);
  tlb_flush(virt_addr);
}

void platform_kmap(uint32_t phys, uint32_t virt, int rw) {
  native_map_virtual((uint32_t)kernel_page_dir, phys, virt, 0, rw);
}

void platform_umap(uint32_t pgd, uint32_t phys, uint32_t virt, int rw) {
  native_map_virtual(pgd, phys, virt, 1, rw);
}
