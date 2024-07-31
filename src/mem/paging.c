#include <catk/virt.h>
#include <catk/debug.h>
#include <lib/common.h>
#include <stdint.h>

#define PAGE_SIZE 4096

/* To be revisited..

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
  uint32_t pgt = PAGE_ADDR((uint32_t)phys_addr) | PTE_IGNORED | PTE_CACHING | user ? PTE_USERMODE : 0 | rw ? PTE_RW : 0 | PTE_PRESENT;
  return pgt;
}

static uint32_t create_pde(void * phys_addr, uint8_t rw, uint8_t user)
{
  uint32_t pgt = PAGE_ADDR((uint32_t)phys_addr) | PTE_IGNORED | PTE_CACHING | user ? PTE_USERMODE : 0 | rw ? PTE_RW : 0 | PTE_PRESENT;
  return pgt;
}

*/

void paging_init(void)
{
  /*
  setup_page_tables();
  load_page_directory((uint32_t)kernel_pagedir);
  */
}
