MEMORY
{
	ROM (rx) :  ORIGIN = 0x102C7040, LENGTH = 0x0005A000
	RAM (rwx) : ORIGIN = 0xf03D2000, LENGTH = 0x00019000
}

SECTIONS
{
    . = 0x102C7040;
	.initdata : ALIGN(4)
	{ 
		KEEP(*(.initdata));
	} AT > ROM	

	text : ALIGN(4)
	{         
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP( *(SORT(.preinit_array.*)) )
        KEEP( *(.preinit_init_array*) )
        PROVIDE_HIDDEN (__preinit_array_end = .);
	
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP( *(SORT(.init_array.*)) )
        KEEP( *(.init_array*) )
        PROVIDE_HIDDEN (__init_array_end = .);
        
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP( *(SORT(.fini_array.*)) )
        KEEP( *(.fini_array*) )
        PROVIDE_HIDDEN (__fini_array_end = .);  
		
        *( .text .text.* i.* )
        *( .glue_7t )
        *( .glue_7 )
        *( .ctors )
        *( .dtors )
        *( .gnu.linkonce.t.* )
        *( .gnu.linkonce.r.* )
        *( .gcc_except_table )
        *( .rodata .rodata* .constdata* .conststring*  )      
	} AT > ROM

	.ARM.exidx : ALIGN(8)
	{ 
	    __exidx_start = .;
	    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
	    __exidx_end = .;
	} AT > ROM	

	.data : ALIGN(8)
	{
		__data_load = LOADADDR(.data);
		__data_start = .;
		*(.data)
        *(.data.*)
		. = ALIGN (8);
		_edata = .;
	} > RAM  AT > ROM

	.bss : ALIGN(8)
	{
		__bss_start__ = .;
		*(.bss)
        *(.bss.*)
		. = ALIGN (8);
		__bss_end__ = .;
	} > RAM
}