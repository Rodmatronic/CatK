section .text
  global _start

section .data
  message: db "Hello World from CatK!", 0

_start:
  mov eax, 0x04
  mov ebx, message
  mov ecx, 0x17
  mov edx, 0x0
  int 0x80

  mov eax, 0x01
  mov ebx, 0xdeadbeef
  int 0x80
  
  jmp $