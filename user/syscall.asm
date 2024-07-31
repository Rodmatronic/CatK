section .text
  global print
  global open

print:
  push ebp
  mov ebp, esp

  mov ebx, [ebp + 8]
  mov eax, 0x03
  int 0x80

  pop ebp
  ret

open:
  push ebp
  mov ebp, esp

  mov eax, 0x04
  mov ebx, [ebp + 8]
  mov ecx, [ebp + 12]
  mov edx, [ebp + 16]
  int 0x80

  pop ebp
  ret