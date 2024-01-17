ENTRY(_start)

SECTIONS
{
	. = 2M;
	__kernel_section_start = .;

	.text 0x0100000 : 
	{
		/* link the multiboot struct here */
		. = ALIGN(8);
		KEEP(*(.multiboot))
		/* place all code afterwards */
		*(.text)
		. = ALIGN(4096);
		__kernel_text_section_start = .;  /* Define symbol for kernel text start */
	}
 
	.rodata ALIGN(4K) : 
	{
		*(.rodata)
		. = ALIGN(4096);
		__kernel_rodata_section_start = .;  /* Define symbol for rodata start */
	}
 
.data : {
    __kernel_data_section_start = .;
    data = .; _data = .; __data = .;
    *(.data)
    . = ALIGN(4096);
    __kernel_data_section_end = .;
}

 
	.bss ALIGN(4K) : 
	{
		*(COMMON)
		*(.bss)
		. = ALIGN(4096);
		__kernel_bss_section_start = .;  /* Define symbol for bss start */
	} 
}

__kernel_text_section_end = .;  /* Define symbol for kernel text end */
__kernel_rodata_section_end = .;  /* Define symbol for rodata end */
__kernel_data_section_end = .;  /* Define symbol for data end */
__kernel_bss_section_end = .;  /* Define symbol for bss end */
__kernel_section_end = .;  /* Define symbol for bss end */
