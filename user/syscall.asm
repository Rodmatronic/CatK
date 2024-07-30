section .text
  global print

print:
  push ebp
  mov ebp, esp

  mov ebx, [ebp + 8]
  mov eax, 0x03
  int 0x80

  pop ebp
  ret