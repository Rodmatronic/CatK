section .text
  global _start:


_start:
  call main
  mov ebx, eax
  xor eax, eax
  int 0x80
  jmp $
