prog_start:
  mov eax, 0x01 ; sys_exit
  mov ebx, 0x80 ; exit code should be 128 in decimal
  int 0x80      ; invoke syscall
  jmp $         ; loop until we're inevitably killed
