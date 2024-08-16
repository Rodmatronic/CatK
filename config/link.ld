ENTRY(_start)

SECTIONS
{
  . = 2M;
  kernel_start = .;
  /* Code */
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.text)
	}
 
	/* Read-only data. */
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}
 
	/* Read-write data (initialized) */
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}
 
	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K)
	{
    bss_start = .;
		*(COMMON)
		*(.bss)
    bss_end = .;
	}
  /DISCARD/ : {
    *(.comment)
    *(.eh_frame)
    *(.rel.eh_frame)
  }
  kernel_end = .;
}
