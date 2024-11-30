#include <multiboot2.h>
#include <catk/debug.h>
#include <lib/common.h>
#include <stdint.h>

static uintptr_t info_ptr;

#define MBI_UNALIGNED(mbi) (mbi & 7)

int multiboot2_validate_mbi(uintptr_t addr)
{
  return (MBI_UNALIGNED(addr) == false);
}

void * multiboot2_locate_tag(uintptr_t addr, int type)
{
  debug("multiboot2_locate_tag: attempting to locate tag %d...\n", type);
  struct multiboot_header_tag * tag = (struct multiboot_header_tag *)(uintptr_t)(addr + 8);
  while(1)
  {
    if(!tag->type)
      return NULL;
    if(tag->type == type)
      return (void *)tag;

    tag = (struct multiboot_header_tag *)((uint8_t *) tag + ((tag->size + 7) & ~7));
  }
}

void multiboot2_set_mbi(uintptr_t mbi) {
  info_ptr = mbi;
}

uintptr_t multiboot2_get_mbi(void) {
  return info_ptr;
}
