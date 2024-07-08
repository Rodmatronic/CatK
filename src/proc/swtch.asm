section .text
  global usermode_switch

saved_eip: dd 0

usermode_switch: ; jumps to a tasks entry-point through a jmp instruction
  cli

  mov eax, [esp + 4]
  mov [saved_eip], eax

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
  mov eax, [saved_eip]
  push dword eax
  iretd
