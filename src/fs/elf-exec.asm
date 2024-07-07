section .text
  global enter_task_from_jmp

saved_eax: dd 0

enter_task_from_jmp:
  mov dword [saved_eax], eax
  mov ax, 0x23
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  mov eax, esp
  push dword 0x23
  push dword eax
  pushfd
  push dword 0x1b
  mov eax, [saved_eax]
  push eax
  iretd
