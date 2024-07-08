section .text
  global open

open:
  push ebp
  mov ebp, esp

  mov eax, 0x04
  mov ebx, [ebp + 8]
  xor ecx, ecx
  xor edx, edx
  int 0x80

  leave
  ret
