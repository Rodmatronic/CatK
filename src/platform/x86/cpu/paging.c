#include <catk/debug.h>
#include <catk/printk.h>
#include <catk/compiler.h>
#include <catk/mem.h>
#include <catk/platform.h>
#include <catk/console.h>
#include <lib/common.h>
#include <stdint.h>

#include "paging.h"

/* page tables */
static uint32_t page_table[1024] _aligned(PAGE_ALIGNMENT);
static uint32_t heap_page_table[1024] _aligned(PAGE_ALIGNMENT);
static uint32_t heap_page_table2[1024] _aligned(PAGE_ALIGNMENT);
static uint32_t fbcon_page_table[1024] _aligned(PAGE_ALIGNMENT);
/* page directories */
static uint32_t kernel_page_dir[1024] _aligned(PAGE_ALIGNMENT);

static inline void tlb_flush(uint32_t v_addr) {
  asm volatile("invlpg (%0)" :: "r"(v_addr) : "memory");
}

uint32_t get_kernel_pgd(void) {
  return (uint32_t)kernel_page_dir;
}

static inline uint32_t get_fault_addr(void) {
  register uint32_t cr2;
  asm volatile("mov %%cr2, %0" : "=r"(cr2));
  return cr2;
}

static void page_fault_handler(struct intr_stack_frame * regs) {
  if(!is_console_enabled()) {
    critical_enter();
    halt();
  }
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
  uint32_t * pgd = copy_pgd(get_kernel_pgd());
  memset(pgd, 0, PAGE_SIZE);
  return pgd;
}

void setup_paging(void)
{
  /* clear page stuff */
  memset(&kernel_page_dir, 0, PAGE_SIZE);
  memset(&page_table, 0, PAGE_SIZE);
  memset(&heap_page_table, 0, PAGE_SIZE);
  memset(&heap_page_table2, 0, PAGE_SIZE);
  memset(&fbcon_page_table, 0, PAGE_SIZE);
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
  p_addr = get_console()->data->vc_screenbuf;
  size = 0x400000;
  for(int i = 0; size > 0; p_addr += PAGE_ALIGNMENT, size -= PAGE_ALIGNMENT, i++) {
    fbcon_page_table[i] = create_pte(p_addr, 0, 1);
  }
  kernel_page_dir[0] = create_pde((uint32_t)page_table, 0, 1);
  kernel_page_dir[1] = create_pde((uint32_t)heap_page_table, 0, 1);
  kernel_page_dir[2] = create_pde((uint32_t)heap_page_table2, 0, 1);
  kernel_page_dir[3] = create_pde((uint32_t)fbcon_page_table, 0, 1);
  native_load_pagedir((uint32_t)kernel_page_dir);
  native_enable_paging();
  get_console()->data->vc_screenbuf = 0xc00000;
  intr_add_handler(0x0e, page_fault_handler);
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

void platform_kmap(uint32_t phys, uint32_t virt, int usermode, int rw) {
  native_map_virtual(get_kernel_pgd(), phys, virt, usermode, rw);
}

void platform_umap(uint32_t cr3, uint32_t phys, uint32_t virt, int usermode, int rw) {
  native_map_virtual(cr3, phys, virt, usermode, rw);
}
