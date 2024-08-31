ENTRY(kentry)

SECTIONS
{
  . = 2M;
  kstart = .;
  /* Code */
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
    ktext_start = .;
		*(.text)
    ktext_end = .;
	}
 
	/* Read-only data. */
	.rodata BLOCK(4K) : ALIGN(4K)
	{
    krodata_start = .;
		*(.rodata)
    krodata_end = .;
	}
 
	/* Read-write data (initialized) */
	.data BLOCK(4K) : ALIGN(4K)
	{
    kdata_start = .;
		*(.data)
    kdata_end = .;
	}
 
	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K)
	{
    kbss_start = .;
		*(COMMON)
		*(.bss)
    kbss_end = .;
	}
	/* useless mumbo jumbo */
  /DISCARD/ : {
    *(.comment)
    *(.eh_frame)
    *(.rel.eh_frame)
  }
  kend = .;
}
