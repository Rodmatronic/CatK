section .text
  global jmp_to_elf

jmp_to_elf:
  mov eax, [ebp + 8]
  jmp [eax]
