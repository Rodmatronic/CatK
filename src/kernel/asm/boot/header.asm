; header.asm
; Defines the Multiboot2 header

HEADER_MAGIC equ 0xe85250d6
BOOTLOADER_MAGIC equ 0x36d76289
KERNEL_ARCH equ 0x0 ; x86 architecture
HEADER_LENGTH equ multiboot2_header_end - multiboot2_header_start
HEADER_CHECKSUM equ -(HEADER_MAGIC + KERNEL_ARCH + (HEADER_LENGTH))

%include "config.inc"

multiboot2_header_start:
  ; Header requires alignment
  align 8
  ; Header magic
  dd HEADER_MAGIC
  ; ISA: i386
  dd KERNEL_ARCH
  ; Header length.
  dd HEADER_LENGTH
  ; Header checksum
  dd HEADER_CHECKSUM
%if CATK_VIDEO_GENERIC == 0
multiboot2_fb_tag_start:
  align 8
  ; Framebuffer tag
  dw 5
  ; Optional tag
  dw 1
  ; Tag length
  dd multiboot2_fb_tag_end - multiboot2_fb_tag_start
  ; Width
  dd 800 ; 640 to 600 might be the magic sigma number to set the resolution to the max.
  ; Height
  dd 600
  ; Bits Per Pixel (BPP)
  dd 32
multiboot2_fb_tag_end:
%endif
multiboot2_header_tag_end:
  align 8
  ; Header end
  dw 0
  dw 0
  dd 8
multiboot2_header_end:
