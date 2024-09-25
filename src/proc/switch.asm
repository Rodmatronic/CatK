section .text
  global usermode_switch

usermode_switch:
  cli

  mov ax, 0x23
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  mov eax, esp
  push dword 0x23  ; ds
  push dword eax   ; esp
  push dword 0x200 ; eflags
  push dword 0x1b  ; cs
  mov eax, [esp + 0x10]
  push eax
  iretd
