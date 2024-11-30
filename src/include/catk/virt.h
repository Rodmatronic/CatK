#ifndef __VIRT_H
#define __VIRT_H

#include <catk/compiler.h>
#include <lib/common.h>

/* For more info about good old paging, download the Intel Software Developer Manual at https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html */

/* We use 4MB page directory :) */

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

#define PAGE_ALIGNMENT      0x1000
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

#define FRAMEBUFFER_VIRT_ADDR 0xc00000

static inline int pde_is_present(uint32_t pde) {
  return (pde & (1 << PDE_PRESENT_SHIFT));
}

static inline int pte_is_present(uint32_t pte) {
  return pde_is_present(pte);
}

static inline uint16_t pde_index(uint32_t v_addr) {
  return (uint16_t)(v_addr >> 22);
}

static inline uint16_t pte_index(uint32_t v_addr) {
  return (uint16_t)((v_addr >> 12) & (1024 - 1));
}

static inline uint32_t pde_virt(uint16_t index) {
  return (index << 22);
}

static inline uint32_t pte_virt(uint16_t index) {
  return (index << 12);
}

static inline uint32_t extract_pte(uint32_t pde) {
  return ((pde) & ~(PAGE_SIZE - 1));
}

/* page operations */
void paging_init(void);
void kvm_map(uint32_t phys_addr, uint32_t virt_addr);
void kvm_unmap(uint32_t virt_addr);
void uvm_map(uint32_t pgd, uint32_t phys_addr, uint32_t virt_addr, uint8_t rw);
void vmm_map_user_region(uint32_t * pgd, uint32_t phys_addr, uint32_t virt_addr, size_t size, uint8_t rw);
void vmm_map_kernel_region(uint32_t phys_addr, uint32_t virt_addr, size_t size);
uint32_t create_new_pgd(void);
uint32_t get_kernel_pgd(void);

#endif
