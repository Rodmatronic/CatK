ENTRY(_start)

SECTIONS
{
  .text : {
    *(.text)
  }
  .data : {
    *(.data)
  }
  .rodata : {
    *(.rodata)
  }
  .bss : {
    *(.bss)
  }
  /DISCARD/ : {
    *(.comment)
    *(.eh_frame)
    *(.rel.eh_frame)
  }
}
