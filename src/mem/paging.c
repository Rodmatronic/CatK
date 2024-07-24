#include <catk/virt.h>
#include <catk/debug.h>
#include <lib/common.h>
#include <stdint.h>

#define PAGE_SIZE 4096

/* stores all the data of the page tables */
static uint32_t kernel_pagedir[1024] _aligned(PAGE_SIZE);
/* contains physical addresses, and info */
static uint32_t kernel_pagetbl[1024] _aligned(PAGE_SIZE);
/* the heap uses 4mb of memory so we only need one of these */
static uint32_t heap_pagetbl[1024] _aligned(PAGE_SIZE);

static inline uint32_t get_pte_index(uint32_t virt)
{
  return (virt >> 12) & 0x3ff;
}

static inline uint32_t get_pde_index(uint32_t virt)
{
  return (virt >> 22);
}

static uint32_t create_pte(void * phys_addr, uint8_t rw, uint8_t user)
{
  /* Isn't caching fast? Why not enable it? */
  uint32_t pgt = PAGE_ADDR((uint32_t)phys_addr) | PTE_IGNORED | PTE_CACHING | user ? PTE_USERMODE : 0 | rw ? PTE_RW : 0 | PTE_PRESENT;
  return pgt;
}

static uint32_t create_pde(void * phys_addr, uint8_t rw, uint8_t user)
{
  /* Isn't caching fast? Why not enable it? */
  uint32_t pgt = PAGE_ADDR((uint32_t)phys_addr) | PTE_IGNORED | PTE_CACHING | user ? PTE_USERMODE : 0 | rw ? PTE_RW : 0 | PTE_PRESENT;
  return pgt;
}

void load_page_directory(uint32_t pagedir)
{
  uint32_t cr0;
  asm volatile("mov %0, %%cr3" ::"r"((uint32_t)pagedir));
  // set bit in cr0 to enable paging
  asm volatile("mov %%cr0, %0": "=r"(cr0));
  cr0 = cr0 | 0x80000001;
  asm volatile("mov %0, %%cr0" ::"r"(cr0));
}

static void setup_page_tables(void)
{
  /* map 4mb for the heap */
  int i;
  for(i = 0; i < 4 * 0x100000; i += PAGE_SIZE)
    heap_pagetbl[get_pte_index(i * PAGE_SIZE)] = create_pte((void *)(i * PAGE_SIZE), 1, 0);

  kernel_pagedir[0] = create_pde(&heap_pagetbl, 1, 0);

  for(; i < 8 * 0x100000; i += PAGE_SIZE)
    kernel_pagetbl[get_pte_index(i * PAGE_SIZE)] = create_pte((void *)(i * PAGE_SIZE), 1, 0);

  kernel_pagedir[get_pde_index(4 * 0x100000)] = create_pde(kernel_pagetbl, 1, 0);
}

void paging_init(void)
{
  /*
  setup_page_tables();
  load_page_directory((uint32_t)kernel_pagedir);
  */
}
