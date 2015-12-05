#define ASM_FILE 1
#include "multiboot.h"

.file "boot.S"

/* Declare constants used for creating a multiboot header. */
.set MB_HEADER_FLAGS, MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
.set MB_CHECKSUM, -(MULTIBOOT_HEADER_MAGIC + MB_HEADER_FLAGS)

/*
 * Declare a header as in the Multiboot Standard. We put this into a special
 * section so we can force the header to be in the start of the final program.
 * You don't need to understand all these details as it is just magic values that
 * is documented in the multiboot standard. The bootloader will search for this
 * magic sequence and recognize us as a multiboot kernel.
 */
.section .multiboot_header
.align MULTIBOOT_INFO_ALIGN
.long MULTIBOOT_HEADER_MAGIC
.long MB_HEADER_FLAGS
.long MB_CHECKSUM

/*
 * Currently the stack pointer register (esp) points at anything and using it may
 * cause massive harm. Instead, we'll provide our own stack. We will allocate
 * room for a small temporary stack by creating a symbol at the bottom of it,
 * then allocating 8192 bytes for it, and finally creating a symbol at the top.
 */
.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 8192
stack_top:

/*
 * The linker script specifies _start as the entry point to the kernel and the
 * bootloader will jump to this position once the kernel has been loaded. It
 * doesn't make sense to return from this function as the bootloader is gone.
 */
.section .text
.global _start
.type _start, @function

_start:
	/* BIOS has enabled interrupts; disable them before we start */
	cli

	/* our stack (as it grows downwards). */
	movl $stack_top, %esp

	/* Push the pointer to the Multiboot information structure. */
	pushl   %ebx

	/* Push the magic value. */
	pushl   %eax

	call kernel_main
	hlt
.hang:
	jmp .hang

/*
 * Set the size of the _start symbol to the current location '.'
 * minus its start.
 * This is useful when debugging or when you implement call tracing.
 */
.size _start, . - _start
