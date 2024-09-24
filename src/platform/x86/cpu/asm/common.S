.section .text
  .global native_gdt_load
  .global native_idt_load

native_gdt_load:
  movl 4(%esp), %eax
  lgdt (%eax)

  movw $0x10, %ax
  movw %ax, %ds
  movw %ax, %es
  movw %ax, %fs
  movw %ax, %gs
  movw %ax, %ss
  jmp $0x08, $gdt_load_far_jmp

gdt_load_far_jmp:
  ret

native_idt_load:
  /* load IDT */
  movl 4(%esp), %eax
  lidt (%eax)
  /* enable interrupts */
  sti
  ret
