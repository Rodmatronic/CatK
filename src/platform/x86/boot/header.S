.section .multiboot

#define ASM_FILE 1

#include <multiboot2.h>

/* each tag must have an 8 byte alignment */

multiboot2_header_begin:
  .align 8
  .long MULTIBOOT2_HEADER_MAGIC
  .long MULTIBOOT_ARCHITECTURE_I386
  .long multiboot2_header_end - multiboot2_header_begin
  .long -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT_ARCHITECTURE_I386 + (multiboot2_header_end - multiboot2_header_begin))
multiboot2_framebuffer_tag_start:
  .align 8
  .short MULTIBOOT_HEADER_TAG_FRAMEBUFFER
  .short 0
  .long multiboot2_framebuffer_tag_end - multiboot2_framebuffer_tag_start
  .long 600
  .long 480
  .long 32
multiboot2_framebuffer_tag_end:
multiboot2_end_tag:
  .align 8
  .short MULTIBOOT_HEADER_TAG_END
  .short 0
  .long 8
multiboot2_header_end: