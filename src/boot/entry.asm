; Multiboot2 and GRUB do not provide a stack for the kernel
; So we have to create our own

section .text
  global kentry
  extern sse_setup
  extern kernel_hang
  extern kernel_crt0
  ; .bss
  extern kbss_start
  extern kbss_end
  
kentry:
	mov esp, stack_top
  ; Reset the EFLAGS register
  push dword 0
  popfd
  ; Check magic
  cmp eax, 0x36d76289
  jne kernel_hang
  ; clear .bss section
  cld
  mov edi, kbss_start
  mov ecx, kbss_end
  sub ecx, kbss_start
  xor eax, eax
  rep stosb
  ; Push kernel arguments
  push ebx
  ; setup ebp for stack unwinding
  xor ebp, ebp
  ; setup SSE2
  call sse_setup
  ; call the kernel
	call kernel_crt0
  ; To make sure the kernel doesn't execute memory bytes thinking that
  ; they're x86 opcodes, we halt the kernel to prevent Invalid Opcode
  ; exceptions
kernel_hang:
  cli
  hlt

section .bss
stack_bottom:
  resb 16384
stack_top:
