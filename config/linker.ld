ENTRY(_start)

SECTIONS
{
	. = 2M;
 
	.text ALIGN(4K) : {
	/* link the multiboot struct here */
	. = ALIGN(8);
	KEEP(*(.multiboot))
	/* place all of your code afterwards */
	*(.text)
	}

 	.rodata ALIGN(4K) : 
	{
		*(.rodata)
	}

	.rodata* ALIGN(4K) : 
	{
		*(.rodata*)
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
