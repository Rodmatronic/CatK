#include <catk/virt.h>
#include <catk/debug.h>
#include <catk/compiler.h>
#include <catk/mem.h>
#include <catk/math.h>
#include <catk/core.h>
#include <catk/kernel.h>
#include <catk/ipc.h>
#include <lib/common.h>
#include <stdint.h>

void * vmm_alloc_pages(size_t pages) {
  if(pages < 1) {
    return NULL;
  }
  debug("Allocating %d virtual memory pages\n", pages);
  void * ptr = physmem_alloc_blocks(pages);
  for(size_t i = 0; i < pages; i++) {
    kvm_map((uint32_t)&ptr + (i * PAGE_SIZE), (uint32_t)&ptr + (i * PAGE_SIZE));
  }
  return ptr;
}

void vmm_free_pages(void * ptr, size_t pages) {
  if(pages < 1) {
    return;
  }
  debug("Freeing %d virtual memory pages at 0x%08x\n", pages, ptr);
  physmem_free_blocks(ptr, pages);
  for(size_t i = 0; i < pages; i++) {
    kvm_unmap((uint32_t)&ptr + (i * PAGE_SIZE));
  }
}

void vmm_map_user_region(uint32_t * pgd, uint32_t phys_addr, uint32_t virt_addr, size_t size, uint8_t rw) {
  size_t pages = DIV_ROUND_UP(size, PAGE_SIZE);
  debug("Mapping %d pages from 0x%08x to 0x%08x to user\n", pages, phys_addr, virt_addr);
  for(size_t i = 0; i < pages; i++) {
    uvm_map((uint32_t)pgd, phys_addr + (i * PAGE_SIZE), virt_addr + (i * PAGE_SIZE), rw);
  }
}

void vmm_map_kernel_region(uint32_t phys_addr, uint32_t virt_addr, size_t size) {
  size_t pages = DIV_ROUND_UP(size, PAGE_SIZE);
  debug("Mapping %d pages from 0x%08x to 0x%08x to kernel\n", pages, phys_addr, virt_addr);
  for(size_t i = 0; i < pages; i++) {
    kvm_map(phys_addr + (i * PAGE_SIZE), virt_addr + (i * PAGE_SIZE));
  }
}
