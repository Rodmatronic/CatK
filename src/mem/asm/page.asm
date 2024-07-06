section .text
  global paging_enable

paging_enable:
  push ebp
  mov ebp, esp

  mov eax, cr0
  or eax, 0x80000001 ; PE (Paging Enable) bit
  mov cr0, eax

  leave
  ret
