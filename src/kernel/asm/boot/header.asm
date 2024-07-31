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
  ; Framebuffer tag
  dw 5
  ; Optional tag
  dw 1
  ; Tag length
  dd multiboot2_fb_tag_end - multiboot2_fb_tag_start
  ; Width
  dd 1280
  ; Height
  dd 720
  ; Bits Per Pixel (BPP)
  dd 32
multiboot2_fb_tag_end:
%endif
multiboot2_header_tag_end:
  ; Header end
  dw 0
  dw 0
  dd 0
multiboot2_header_end:
