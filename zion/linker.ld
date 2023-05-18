OUTPUT_FORMAT(elf64-x86-64)

ENTRY (zion)

/* Define the program headers we want so the bootloader gives us the right */
/* MMU permissions */
PHDRS
{
    text    PT_LOAD    FLAGS((1 << 0) | (1 << 2)) ; /* Execute + Read */
    rodata  PT_LOAD    FLAGS((1 << 2)) ;            /* Read only */
    data    PT_LOAD    FLAGS((1 << 1) | (1 << 2)) ; /* Write + Read */
}

SECTIONS
{
  . = 0xffffffff80000000;
	/* Add a symbol that indicates the start address of the kernel. */
	_kernel_start = .;

	.text : {
			*(.text .text.*)
	} :text

	/* Move to the next memory page for .rodata */
	. += CONSTANT(MAXPAGESIZE);

	.rodata : {
			*(.rodata .rodata.*)
	} :rodata

	/* Move to the next memory page for .data */
	. += CONSTANT(MAXPAGESIZE);

	.data : {
			*(.data .data.*)
	} :data

	.bss : {
			*(COMMON)
			*(.bss .bss.*)
	} :data
	/* Add a symbol that indicates the end address of the kernel. */
	_kernel_end = .;
}
