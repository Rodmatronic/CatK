#include <stdint.h>
#include <catk/mem.h>
#include <catk/printk.h>
#include <sys/types.h>
#include <catk/spinlock.h>
#include <lib/common.h>

SPINLOCK_INIT(jpalloc_spinlock);

#pragma GCC diagnostic ignored "-Wmultichar" 

// I still dont really got the kernel memory map fully figured out
//
// [        2MB        ][                 1MB                 ][                        Reserved for user-mode stuff                        ]
//
// ^                    ^
// Kernel Image         Kernel Heap                

struct heap_metadata
{
  uintptr_t size;
  bool used;
};

struct heap_block
{
  uintptr_t magic;
  struct heap_metadata metadata;
};

#define KERNEL_HEAP_MAX         0x300000
#define KERNEL_HEAP_MAGIC       'heap'
#define KERNEL_HEAP_ALIGNMENT   8

#define IN_HEAP_RANGE(ptr) ((uintptr_t)&ptr >= heap_start && (uintptr_t)&ptr <= heap_end)

static bool heap_initialized = false;
static uintptr_t heap_start = 0;
static uintptr_t heap_end, heap_used = 0;
static uintptr_t prev_alloc = 0;

void heap_init(uintptr_t * start)
{
  heap_start = (uintptr_t)start;
  prev_alloc = (uintptr_t)heap_start;
  heap_end = (uintptr_t)(heap_start + KERNEL_HEAP_MAX);
  memset((char *)heap_start, 0, heap_end - heap_start);
}

uintptr_t heap_get_used(void)
{
  return heap_used; 
}

static inline size_t align(size_t size)
{
  return ALIGN(KERNEL_HEAP_ALIGNMENT, size);
}

static void * heap_get_free(size_t size)
{
  struct heap_block * block = NULL;
  uintptr_t * mem = (uintptr_t *)heap_start;

  size = align(size);
  // if this is our first alloc, return the start of the kernel heap
  if (heap_used == 0)
  {
    return (void *)prev_alloc;
  }

  while ((uintptr_t)mem < heap_end)
  {
    block = (struct heap_block *)mem;
    if (block->magic != KERNEL_HEAP_MAGIC || !block->metadata.size)
    {
      return mem;
    }

    if (!block->metadata.used && block->metadata.size >= size)
    {
      return mem;
    }

    mem += (block->metadata.size + sizeof(struct heap_block)) / sizeof(uintptr_t);
  }
  return NULL;
}

static void * heap_alloc(size_t size)
{
  spinlock_acquire(&jpalloc_spinlock);
  if(!size)
    return NULL;
  struct heap_block * b = NULL;
  b = heap_get_free(size);
  if(!b)
  {
    return NULL;
  }
  size_t usage = prev_alloc + size + sizeof(struct heap_block);
  if(usage > heap_end)
  {
    return NULL;
  }

  // update variables
  
  prev_alloc += size + sizeof(struct heap_block);

  heap_used += size + sizeof(struct heap_block);
  // update header
  b->magic = KERNEL_HEAP_MAGIC;
  b->metadata.size = size;
  b->metadata.used = true;
  spinlock_release(&jpalloc_spinlock);
  return (void *)((uintptr_t)b + sizeof(struct heap_block));
}

static inline struct heap_block * get_header_from_ptr(void * ptr)
{
  return (struct heap_block *)(ptr - sizeof(struct heap_block));
}

static void heap_free(void * ptr)
{
  /* do we have a valid pointer? */
  if(!ptr)
    return;

  struct heap_block * b = get_header_from_ptr(ptr);

  if(b->magic != KERNEL_HEAP_MAGIC)
    return;
  
  if(!b->metadata.used)
    return;

  if(!IN_HEAP_RANGE(ptr))
    return;

  // update variables
  b->metadata.used = false;
  heap_used -= b->metadata.size + sizeof(struct heap_block);
}

static void heap_merge_blocks(void)
{
  uintptr_t * mem = (uintptr_t *)heap_start;
  struct heap_block * prev = NULL;

  while ((uintptr_t)mem < heap_end)
  {
    struct heap_block * block = (struct heap_block *)mem;

    /* is a valid block? */
    if (block->magic != KERNEL_HEAP_MAGIC)
      break;
    
    /* found two free blocks, now, we merge! */
    if (prev && !prev->metadata.used && !block->metadata.used)
    {
      prev->metadata.size += block->metadata.size + sizeof(struct heap_block);
    }
    else
    {
      prev = block; /* save block and look for other good ones */
    }

    mem += (block->metadata.size + sizeof(struct heap_block)) / sizeof(uintptr_t);
  }
}

void * malloc(size_t n)
{
	return heap_alloc(n);
}

void free(void * ptr)
{
	heap_free(ptr);
  /* merge free blocks to prevent fragmentation */
  heap_merge_blocks();
}

void * calloc(size_t num, size_t size)
{
  size_t sz = num * size;
  void * ptr = malloc(sz);
  if(!ptr)
    return NULL;
  struct heap_block * b = get_header_from_ptr(ptr);
  memset((void *)((uintptr_t)b + sizeof(struct heap_block)), 0, sz);
  return ptr;
}
