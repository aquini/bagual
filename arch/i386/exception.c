/*
 * From INTEL 80386 PROGRAMMER'S REFERENCE MANUAL 1986:
 * 00  Divide error
 * 01  Debug exception
 * 02  Nonmaskable interrupt
 * 03  Breakpoint (one-byte INT 3 instruction)
 * 04  Overflow (INTO instruction)
 * 05  Bounds check (BOUND instruction)
 * 06  Invalid opcode
 * 07  Coprocessor not available
 * 08  Double fault
 * 09  (reserved)
 * 10  Invalid TSS
 * 11  Segment not present
 * 12  Stack exception
 * 13  General protection
 * 14  Page fault
 * 15  (reserved)
 * 16  Coprecessor error
 *
 * 17-31  (reserved)
 * 32-255 Available for external interrupts via INTR pin
 *
 */

#include "const.h"
#include "types.h"
#include "i386.h"
#include "exception.h"
#include "terminal.h"

/**
 *	The task that calls this routine must be killed.
 *	If it is invoked by the kernel we can only halt
 *	the system!
 */
void panic(void)
{
	cprintf("\nkernel panic: system halted!\n");
	halt();
}

static __INLINE__ void dump_stack(exc_context_t *c)
{
	uint32_t *esp = (uint32_t *)c;
	int i = 0;

	while ((uint32_t *)c) {
		cprintf("[ %p ] %p ", esp, *esp);
		esp++;
		cprintf("%p\n", *esp);
		if (++i >= 8)
			break;
		esp++;
	}
}

static __INLINE__ void dump_registers(exc_context_t *c)
{
	register uint16_t ss;
	register uint32_t cr0, cr2, cr3, cr4;
	uint16_t fs, gs;

	/* Save stack segment register */
	__asm__ __volatile__ ("movw %%ss, %0" : "=&r"(ss) :);

	/* Save %fs and %gs registers */
	__asm__ __volatile__ ("movw %%fs, %0" : "=m"(fs) :);
	__asm__ __volatile__ ("movw %%gs, %0" : "=m"(gs) :);

	/* Save control registers */
	__asm__ __volatile__ ("mov %%cr0, %0" : "=&r"(cr0) :);
	__asm__ __volatile__ ("mov %%cr2, %0" : "=&r"(cr2) :);
	__asm__ __volatile__ ("mov %%cr3, %0" : "=&r"(cr3) :);
	__asm__ __volatile__ ("mov %%cr2, %0" : "=&r"(cr4) :);

	/* Dump registers */
	cprintf("\nCPU REGISTERS:\n");
	cprintf("eax = %08x  ds = %08x  cr0 = %08x  esp    = %08x\n", c->eax, (uint16_t)(c->ds), cr0, c);
	cprintf("ebx = %08x  es = %08x  cr2 = %08x  ebp    = %08x\n", c->ebx, (uint16_t)(c->es), cr2, c->ebp);
	cprintf("ecx = %08x  fs = %08x  cr3 = %08x  eip    = %08x\n", c->ecx, (uint16_t)(fs), cr3, c->eip);
	cprintf("edx = %08x  gs = %08x  cr4 = %08x  eflags = %08x\n", c->edx, (uint16_t)(gs), cr4, c->eflags);
	cprintf("esi = %08x  ss = %08x  exc = %08x\n", c->esi, (uint16_t)(ss), c->exc);
	cprintf("edi = %08x  cs = %08x  err = %08x\n", c->edi, (uint16_t)(c->cs), c->err_code);
	cprintf("STACK:\n");

	/* Dump stack */
	dump_stack(c);
}

void default_exception(exc_context_t *c)
{
	switch (c->exc) {
	case EXC_ZERO_DIV:
	case EXC_DEBUG:
	case EXC_NMI:
	case EXC_BREAKPNT:
	case EXC_OVERFLOW:
	case EXC_BOUND:
	case EXC_INV_OP_CODE:
	case EXC_PROC_EXT:
	case EXC_DOUBLE_FAULT:
	case EXC_PROC_EXT_FAULT:
	case EXC_INVALID_TSS:
	case EXC_SEGMENT_INV:
	case EXC_STACK_FAULT:
	case EXC_GEN_PROT_FAULT:
	case EXC_PAGE_FAULT:
	case EXC_UNKNOWN:
		cprintf("\nException 0x%x: %s\n",
			c->exc, exception_string[c->exc]);
		break;
	default:
		cprintf("\nException %x: UNEXPECTED !!!\n", c->exc);
		break;
	}

	dump_registers(c);
	panic();
}
