#include <catk/printk.h>
#include <catk/mem.h>
#include <catk/debug.h>
#include <catk/kernel.h>
#include <catk/bitops.h>
#include <multiboot2.h>
#include <lib/common.h>
#include <lib/bitmap.h>
#include <stdint.h>

/* 
* We can forget about the using linear allocation for the PMM..
* Using a bitmap allocator is the way to go.
*/

#define PHYSMEM_BLOCK_SIZE 4096
#define PHYSMEM_BLOCK_USED 0b11111111

static uint8_t * pmm_bitmap = NULL;
static uint32_t pmm_max_blocks = 0;

/* 1 = used, 0 = free */

void physmem_init(uintptr_t mbi) {
  int i = 0;
  struct multiboot_tag_mmap * mmap = (struct multiboot_tag_mmap *)multiboot2_locate_tag(mbi, MULTIBOOT_TAG_TYPE_MMAP);
  struct multiboot_header_tag * tag = (struct multiboot_header_tag *)mmap;
  struct multiboot_mmap_entry * mmap_entry = NULL;
  for(mmap_entry = mmap->entries; (uint8_t *)mmap_entry < (uint8_t *)tag + tag->size; mmap_entry = (multiboot_memory_map_t *)((uint32_t)mmap_entry + mmap->entry_size)) {
    uint32_t addr = (uint32_t)(mmap_entry->addr & 0xffffffff);
    uint32_t addr_len = (uint32_t)(mmap_entry->len & 0xffffffff);
    if(mmap_entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
      pmm_bitmap = (uint8_t *)addr;
      pmm_max_blocks = BYTES_TO_BITS(addr_len);
      break;
    }
  }
  assert(pmm_bitmap == NULL);
  /* all memory is unallocated */
  memset(pmm_bitmap, 0, pmm_max_blocks);
}



void * physmem_alloc_block(void) {
  /* Variable I = PMM bitmap index */
  /* Variable J = Bit of bitmap[i] */
  for(int i = 0; i < pmm_max_blocks; i++) {
    if(pmm_bitmap[i] != PHYSMEM_BLOCK_USED) {
      /* check for free block */
      for(int j = 0; j < 8; j++) {
        if((pmm_bitmap[i] & BIT(j)) == 0) {
          bitmap_set(pmm_bitmap, i * 8 + j);
          return (void *)((i * 8 + j) * PHYSMEM_BLOCK_SIZE);
        }
      }
    }
  }
  return NULL;
}

void physmem_free_block(void * block) {
  int n = ((uint32_t)block / PHYSMEM_BLOCK_SIZE) % 8;
  if(bitmap_test(pmm_bitmap, n) == 1)
    bitmap_unset(pmm_bitmap, n);
}
