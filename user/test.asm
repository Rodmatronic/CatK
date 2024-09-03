section .text
  global do_test

section .rodata
  msg: db "Mr Fizzbuzz says hello!", 0

do_test:
  push ebp
  mov ebp, esp

  mov eax, 0x03
  mov ebx, 0x01
  mov ecx, msg
  mov edx, $ - msg
  int 0x80

  leave
  ret
