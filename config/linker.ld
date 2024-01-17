ENTRY(_start)

SECTIONS
{
	. = 2M;
 
	.text 0x0100000 : 
	{
		/* link the multiboot struct here */
		. = ALIGN(8);
		KEEP(*(.multiboot))
		/* place all code afterwards */
		*(.text)
	}
 
	.rodata ALIGN(4K) : 
	{
		*(.rodata)
	}
 
	.data ALIGN(4K) : 
	{
		*(.data)
	}
 
	.bss ALIGN(4K) : 
	{
		*(COMMON)
		*(.bss)
	} 
}