section .text
  global mmu_load_page_directory
  global mmu_enable

mmu_load_page_directory:
  push ebp
  mov ebp, esp

  mov eax, [esp + 8]
  mov cr3, eax
  
  mov esp, ebp
  pop ebp
  ret

mmu_enable:
  push ebp
  mov ebp, esp

  mov eax, cr0
  or eax, 0x80000000
  mov cr0, eax

  mov esp, ebp
  pop ebp
  ret
