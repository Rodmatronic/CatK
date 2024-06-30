#include <multiboot2.h>
#include <lib/common.h>
#include <stdint.h>

int multiboot2_validate_args(uint32_t magic, uint32_t addr)
{
  if(magic != MULTIBOOT2_BOOTLOADER_MAGIC)
    return 0; /* why even bother continuing if we aren't being loaded by GRUB?? */
  if(addr & 7)
    return 0; /* the address is unaligned. never use an unaligned address given by GRUB */
  return 1;
}

void * multiboot2_locate_tag(uintptr_t addr, int type)
{
  //debug("multiboot2_locate_tag: attempting to locate tag %d...\n", type);
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
