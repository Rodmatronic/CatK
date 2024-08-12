section .text
  global gdt_flush
  global idt_flush
  global tss_install
  global get_eip

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
