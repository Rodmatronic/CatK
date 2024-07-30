section .text
  global _start
  extern main

_start:
  call main

  mov ebx, eax
  mov eax, 0x01
  int 0x80
  
  jmp $