section .text
  global gdt_flush
  global idt_flush
  global tss_install
  global get_eip
  global sse_setup
  extern kernel_hang

gdt_flush:
  mov eax, [esp + 4]
  lgdt [eax]

  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax

  jmp 0x08:gdt_flush.end

.end:
  ret

get_eip:
  pop eax ; pop return address from CALL instruction
  jmp eax ; jump to eax (which contains the return address)

idt_flush:
  mov eax, [esp + 4]
  lidt [eax]
  sti
  ret

tss_install:
  pusha

  mov ax, 0x28
  ltr ax

  popa
  ret


sse_setup:
  push ebp
  mov ebp, esp

  pushad

  mov eax, 0x01
  cpuid
  test edx, 1 << 25
  jz kernel_hang
  ; muehehehe copy + paste from osdev.org coming in clutch
  mov eax, cr0
  and ax, ~(1 << 2)
  or ax, 1 << 1
  mov cr0, eax
  mov eax, cr4
  or ax, 3 << 9
  mov cr4, eax

  popad

  mov esp, ebp
  pop ebp
  ret
