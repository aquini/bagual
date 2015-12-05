#ifndef MEM_H
#define MEM_H

/* Number of entries in the GDT (Global Descriptor Table) */
#define GDT_DIM		8192
/* GDT entry dimension in bytes */
#define GDT_ENTRY_DIM	sizeof(gdt_entry_t)

/* Segment Constants */
#define PRESENT		0x80
#define CODE		0x18
#define DATA		0x10
#define EXP_DOWN	0x04
#define CONFORMING	0x04
#define READABLE	0x02
#define WRITEABLE	0x02

/* A standard code segment */
#define CODE_SEG	(CODE | PRESENT | READABLE)
/* A standard data segment */
#define DATA_SEG	(DATA | PRESENT | WRITEABLE)
/* A standard stack segment */
#define STACK_SEG	DATA_SEG
/* A standard TSS segment */
#define TSS_SEG		(PRESENT | 0x09)
/* A real-mode area segment */
#define REAL_SEG	(CODE | PRESENT | READABLE)

#define INT_GATE	(PRESENT | 0x0E) /* 32-bit Interrupt Gate */
#define TASK_GATE	(PRESENT | 0x05) /* Task Gate */
#define TRAP_GATE	(PRESENT | 0x0F) /* 32-bit Trap Gate */

/* Descriptor Privilege Levels (0 = maximum privilege) */
#define DPL_0		0x00
#define DPL_1		0x20
#define DPL_2		0x40
#define DPL_3		0x60

/* The busy bit of the TSS descriptor (to avoid recursive tasks) */
#define BUSY_FLAG	0x02

//! Default size of the istructions (for a code segment) or
//! the stack alignment (for a stack segment).
#define ATTR_32BIT      0x40
//! The granularity of the segment size (limit field):
//! \li	0 => byte granularity;
//! \li 1 => page granularity (4096 bytes).
#define ATTR_GRANUL     0x80

//! This is the data selector for the kernel.
//! REMEBER!!! It is defined also into irq.S!
#define KERNEL_DATA		0x08
//! This is the stack selector for the kernel.
#define KERNEL_STACK		0x08
//! This is the code selector for the kernel.
#define KERNEL_CODE		0x10

//! This is the selector for the user data segment.
#define USER_DATA		0x18
//! This is the selector for the user stack segment.
#define USER_STACK		0x18
//! This is the selector for the user code segment.
#define USER_CODE		0x20

//! This is the selector for the double fault exception task.
#define GDT_DOUBLE_FAULT_ENTRY	0x28

#ifndef ASM_FILE

#include "const.h"

//! (real-mode) Extract the segment part of a linear address.
#define SEGMENT(linear)		( (uint16_t)(((uint32_t)(linear) & 0xFFFF0000) >> 4) )
//! (real-mode) Extract the offset part of a linear address.
#define OFFSET(linear)		( (uint16_t)((uint32_t)(linear) & 0xFFFF) )
//! (real-mode) Make a linear address from a segment:offset address.
#define LINEAR(seg, off)	( (uint32_t)(((uint16_t)(seg) << 4) + (uint16_t)(off)) )

//! (i386) Make a far pointer from \p seg segment and \p off offset.
//! \note In this case a segment is considered as 65536(0x10000) bytes.
#define MK_FP(seg, off)		((uint32_t *) (((uint32_t) (seg) << 16) | (uint16_t) (off)))
//! (i386) Get the segment from the far pointer \p fp.
#define FP_SEG(fp)		(((uint32_t *) fp) >> 16)
//! (i386) Get the offset from the far pointer \p fp.
#define FP_OFF(fp)		(((uint32_t *) fp) & 0xffff)
//! Translate a real-mode segment:offset address to the linear address.
//! This macro is equivalent to LINEAR(seg, off) macro.
#define FP_TO_LINEAR(seg, off)	((void*) ((((uint16_t) (seg)) << 4) + ((uint16_t) (off))))


//! GDT structure.
/** P=Present, DPL=Descriptor Privilege Level, S=Descriptor type (0=system; 1=code or data),
 * Type=Segment type, G=Granularity, D/B=Default operation size(0=16bit; 1=32bit segment),
 * AVL=Available for use by system software.
 */
typedef struct gdt_entry
{
	uint16_t limit;
	uint16_t base0_15;
	uint8_t base16_23;
	uint8_t attribs0_7;  /* P(1bit) | DPL(2bit) | S(1bit) | Type(4bit) */
	uint8_t attribs8_15; /* G(1bit) | D/B(1bit) | 0(1bit) | AVL(1bit) | limit16_19(4bit) */
	uint8_t base24_31;
} __attribute__ ((packed)) gdt_entry_t;

/* The GDT register format */
typedef struct gdt_register
{
	uint16_t limit;
	uint32_t base;
} __attribute__ ((packed)) gdt_reg;

/* Update segment registers with kernel selectors */
static __INLINE__ void k_update_segment_regs()
{
	__asm__ __volatile__(
		"ljmp %0, $1f\n"
		"nop\n"
		"1: mov %1, %%ds\n"
		"mov %1, %%es\n"
		"mov %1, %%ss\n"
		"nop\n"
		"mov %1, %%fs\n"
		"mov %1, %%gs\n"
		: : "i"(KERNEL_CODE), "r"(KERNEL_DATA) );
}

uint16_t setup_gdt_entry(uint16_t limit, uint32_t base, uint8_t attribs0_7, uint8_t attribs8_15);
void remove_gdt_entry(uint16_t sel);
void install_gdt();

#endif /* !ASM_FILE */
#endif /* MEM_H */
