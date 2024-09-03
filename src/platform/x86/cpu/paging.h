#ifndef __PLATFORM_X86_PAGING_H
#define __PLATFORM_X86_PAGING_H

#define PDE_PRESENT_SHIFT   0
#define PDE_RW_SHIFT        1
#define PDE_USERMODE_SHIFT  2
#define PDE_PWT_SHIFT       3
#define PDE_NOCACHE_SHIFT   4
#define PDE_ACCESSED_SHIFT  5
#define PDE_DIRTY_SHIFT     6
#define PDE_PAGESIZE_SHIFT  7
#define PDE_GLOBAL_SHIFT    8
#define PDE_IGNORED         (~BIT(9) | ~BIT(10) | ~BIT(11))

#define PAGE_ALIGNMENT      4096
#define PAGE_SIZE           PAGE_ALIGNMENT

#define PTE_PRESENT_SHIFT   0
#define PTE_RW_SHIFT        1
#define PTE_USERMODE_SHIFT  2
#define PTE_PWT_SHIFT       3
#define PTE_NOCACHE_SHIFT   4
#define PTE_ACCESSED_SHIFT  5
#define PTE_DIRTY_SHIFT     6
#define PTE_PAT_SHIFT       7
#define PTE_GLOBAL_SHIFT    8
#define PTE_IGNORED         PDE_IGNORED

#ifndef ASM_FILE

/* asm functions */

extern void native_load_pagedir(uint32_t pgd);
extern void native_enable_paging(void);

/* c functions */

static inline uint32_t pte_index(uint32_t virt_addr) {
  return (uint32_t)((virt_addr << 12) & PAGE_SIZE - 1);
}

static inline uint32_t pde_index(uint32_t virt_addr) {
  return (uint32_t)(virt_addr << 22);
}

static inline int pde_is_present(uint32_t pde) {
  return (pde & 1 << PDE_PRESENT_SHIFT);
}

static inline int pte_is_present(uint32_t pte) {
  return pde_is_present(pte);
}

static inline int pde_virt(int index) {
  return (index << 22);
}

static inline int pte_virt(int index) {
  return (index << 12);
}

static inline uint32_t extract_pte(uint32_t pde) {
  return ((pde) & ~(PAGE_SIZE - 1));
}

void setup_paging(void);

#endif /* ASM_FILE */

#endif
