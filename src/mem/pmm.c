#include <catk/printk.h>
#include <catk/mem.h>
#include <catk/debug.h>
#include <catk/kernel.h>
#include <catk/bitops.h>
#include <catk/platform.h>
#include <multiboot2.h>
#include <lib/common.h>
#include <lib/bitmap.h>
#include <stdint.h>

/* 
* We can forget about the using a linear allocator for the PMM..
* Using a bitmap allocator is the way to go.
*/

#define PHYSMEM_BLOCK_SIZE 4096
#define PHYSMEM_BLOCK_USED 0b11111111

static uint32_t * pmm_bitmap = NULL;
static uint32_t pmm_bitmap_start;
static uint32_t pmm_max_blocks;
static uint32_t pmm_bitmap_len;

/* 1 = used, 0 = free */

void physmem_init(uintptr_t mbi) {
  struct multiboot_tag_mmap * mmap = (struct multiboot_tag_mmap *)multiboot2_locate_tag(mbi, MULTIBOOT_TAG_TYPE_MMAP);
  struct multiboot_header_tag * tag = (struct multiboot_header_tag *)mmap;
  struct multiboot_mmap_entry * mmap_entry = NULL;
  for(mmap_entry = mmap->entries; (uint8_t *)mmap_entry < (uint8_t *)tag + tag->size; mmap_entry = (multiboot_memory_map_t *)((uint32_t)mmap_entry + mmap->entry_size)) {
    uint32_t addr = (uint32_t)(mmap_entry->addr & 0xffffffff);
    uint32_t addr_len = (uint32_t)(mmap_entry->len & 0xffffffff);
    if(mmap_entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
      if(addr == (uint32_t)&ktext_start) {
        uint32_t addr_end = addr + addr_len;
        pmm_bitmap_start = (uint32_t)&kend + 1024 * 1024;
        pmm_bitmap = (uint32_t *)(pmm_bitmap_start);
        pmm_bitmap_len = (addr_end - pmm_bitmap_start) * sizeof(uint32_t);
        pmm_max_blocks = pmm_bitmap_len / PHYSMEM_BLOCK_SIZE;
        break;
      }
    }
  }
  assert(pmm_bitmap_start != 0);
  /* all memory is unallocated */
  memset(pmm_bitmap, 0, pmm_max_blocks * sizeof(uint32_t));
  for(int i = 0; i < (pmm_max_blocks * sizeof(uint32_t)) / 4096; ++i) {
    uint32_t offset = i * PHYSMEM_BLOCK_SIZE;
    platform_kmap((uint32_t)&pmm_bitmap[pmm_max_blocks] + i, (uint32_t)&pmm_bitmap[pmm_max_blocks] + i, 1);
  }
}

/* not quite sure on how to allocate multiple blocks.. */

void * physmem_alloc_block(void) {
  /* Variable I = PMM bitmap index */
  /* Variable J = Bit of bitmap[i] */
  for(int i = 0; i < pmm_max_blocks; i++) {
    if(pmm_bitmap[i] != PHYSMEM_BLOCK_USED) {
      /* check for free block */
      for(int j = 0; j < 32; j++) {
        if((pmm_bitmap[i] & BIT(j)) == 0) {
          bitmap_set(pmm_bitmap, i * 32 + j);
          return (void *)(((i * 32 + j) * PHYSMEM_BLOCK_SIZE) + (uint32_t)&pmm_bitmap[pmm_max_blocks]);
        }
      }
    }
  }
  return NULL;
}

void physmem_free_block(void * addr) {
  uint32_t block = (uint32_t)addr - (uint32_t)&pmm_bitmap[pmm_max_blocks];
  int n = block / PHYSMEM_BLOCK_SIZE; 
  bitmap_unset(pmm_bitmap, n);
}
