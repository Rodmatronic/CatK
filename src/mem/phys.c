/*

  catk/src/mem/phys.c

  The CatK Project 2023 - 2024
  Author: Foo Bar

  Description:
    Obtains the GRUB memory map and stores blocks into the PMM stack. Also provides useful tools.

*/

/* A stack based approach seems more easier than a bitmap one. I also assume that it's faster. */

#include <catk/mem.h>
#include <catk/math.h>
#include <catk/printk.h>
#include <catk/debug.h>
#include <lib/common.h>
#include <multiboot2.h>
#include <stdint.h>

#define PHYSMEM_MAX_MEM 0x20000000 /* 512 MiB */
#define PHYSMEM_ALLOC_SIZE 4096

#define PHYSMEM_TOTAL_BLOCKS PHYSMEM_MAX_MEM / PHYSMEM_ALLOC_SIZE

/* physical memory manager stack */
static uint32_t pmm_stack[PHYSMEM_TOTAL_BLOCKS]; /* 131,072 blocks */
static uint32_t pmm_stack_head;

static int total_blocks = 0;

/* TODO: optimize */ // << easily solved by removing the debug() functions :P
int physmem_init(uint32_t mbi) {
  /* populate the pmm stack with free addresses obtained by the GRUB memory map */
  struct multiboot_tag_mmap * mmap = (struct multiboot_tag_mmap *)multiboot2_locate_tag(mbi, MULTIBOOT_TAG_TYPE_MMAP);
  struct multiboot_header_tag * tag = (struct multiboot_header_tag *)mmap;
  int i = 0;
  struct multiboot_mmap_entry * mmap_entry = NULL;
  for (mmap_entry = mmap->entries; (uint8_t *)mmap_entry < (uint8_t *)tag + tag->size;
      mmap_entry = (multiboot_memory_map_t *)((uint32_t)mmap_entry + mmap->entry_size)) {
    uint32_t addr = (uint32_t)(mmap_entry->addr & 0xffffffff);
    uint32_t addr_len = (uint32_t)(mmap_entry->len & 0xffffffff);
    if(mmap_entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
      /* split up the address into 4kib blocks */
      addr = ALIGN(PHYSMEM_ALLOC_SIZE, addr);
      while (addr < (mmap_entry->addr + mmap_entry->len)) {
        if (i >= PHYSMEM_TOTAL_BLOCKS) {
          return 0;
        }      
        pmm_stack[i++] = addr;
        addr += PHYSMEM_ALLOC_SIZE;
        total_blocks++;
      }
    }
  } 
  pmm_stack_head = total_blocks;
  return 0;
}

static inline void physmem_stack_push(uint32_t addr) {
  pmm_stack[pmm_stack_head++] = addr;
}

static inline uint32_t physmem_stack_pop(void) {
  uint32_t addr = pmm_stack[--pmm_stack_head];
  return addr;
}

void physmem_block_free(uint32_t addr) {
  /* push */
  physmem_stack_push(addr);
}

/* pop: pmm_stack_head++ */
/* push: --pmm_stack_head */

uint32_t physmem_block_alloc(void) {
  /* pop into 'addr' */
  uint32_t addr = physmem_stack_pop();
  return addr;
}
