#include <catk/platform.h>
#include <catk/printk.h>
#include <catk/mem.h>

/*
void * vmm_alloc_page(int pages) {
  void * addr = physmem_alloc_blocks(pages);
  for(int i = 0; i < pages; ++i) {
    uint32_t off = i * 4096;
    platform_kmap(addr + off, addr + off, 1);
  }
}*/
