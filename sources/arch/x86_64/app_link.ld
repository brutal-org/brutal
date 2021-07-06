
ENTRY(__entry_point)
SECTIONS
{

	. = 4M;
	__start = .;



	.text ALIGN(4K) :
	{
		*(.text)    
        *(.text*)
		*(.init)
		*(.fini)
		*(.ctors)
		*(.dtors)
	}

	.rodata ALIGN(4K) : 
	{
        *(.ctors)
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

	__end = .;
}


