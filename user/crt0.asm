section .text
  global _start
  extern main

_start:
  ; Call C entry-point
  call main
  ; Exit using the kernel
  mov ebx, eax  ; Move the return code to ebx
  mov eax, 0x01 ; sys_exit
  int 0x80      ; Call the kernel
  jmp $         ; Loop forever until killed
