/*
 * generic gcc linker script for LPC810
 * Kamal Mostafa <kamal@whence.com>.
 *
 * License: Public Domain
 */

MEMORY
{
	/* NXP LPC810 */
	FLASH (rx)	: ORIGIN = 0x00000000, LENGTH = 0x00002000 /* 4 Kbytes */
	RAM (rwx)	: ORIGIN = 0x10000000, LENGTH = 0x00000300 /* 768 bytes */
	STACK (rwx)	: ORIGIN = 0x10000300, LENGTH = 0x00000100 /* 256 bytes */
}

SECTIONS
{
	.text :
	{
		_text = .;
		KEEP(*(.isr_vector))
		KEEP(*(.after_vectors))
		*(.text*)
		*(.rodata*)
		. = ALIGN(4);
		_etext = .;
	} > FLASH

	.data : AT (_etext)
	{
		_data = .;
		*(vtable)
		*(.data*)
		_edata = .;
	} > RAM

	.bss :
	{
		_bss = .;
		*(.bss*)
		*(COMMON)
		_ebss = .;
	} > RAM

	end = .;

	.stack :
	{
		_stack_bottom = .;
		. += 256;
		_stack_top = .;
	} > STACK
}
