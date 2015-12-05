#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "const.h"
#include "i386.h"

// --- IRQ ------------------------------------------------------------ //

#define IRQ_NR		16
#define TIMER_IRQ	0x00
#define KEYBOARD_IRQ	0x01
#define RS232_1_IRQ	0x04
#define RS232_2_IRQ	0x03
#define FLOPPY_IRQ	0x06
#define IDE1_IRQ	0x0E
#define IDE2_IRQ	0x0F

//! Minirighi interrupt (used for the system calls).
#define MINIRIGHI_INT	0x80
//! DOS interrupt (maybe will be used for DOS emulation...)
#define DOS_INT		0x20

//! Interrupt enable flag in EFLAGS register.
#define EFLAGS_IF	0x200

//! I/O privilege level 3 (minimum privilege).
//! Everyone can perform I/O operation.
#define EFLAGS_IOPL3	0x3000

//! I/O privilege level 2.
#define EFLAGS_IOPL2	0x2000

//! I/O privilege level 1.
#define EFLAGS_IOPL1	0x1000

//! I/O privilege level 0 (maximum privilege).
//! Only the kernel can perform I/O operation.
#define EFLAGS_IOPL0	0x0000

// --- PIC (Programmable Interrupt Controller) 8259a ------------------ //

//! PIC 8259 master.
#define PORT_8259_M	0x20

//! PIC 8259 slave.
#define PORT_8259_S	0xa0

//! PIC 8259 master (interrupt mask).
#define PORT_INT_MASK_M	0x21

//! PIC 8259 slave (interrupt mask).
#define PORT_INT_MASK_S	0xa1

//! PIC 8259 EOI (End Of Interrupt).
#define EOI		0x20

/* Number of entries in the IDT */
#define IDT_SIZE	256

typedef struct idt_entry {
	uint16_t offset0_15;
	uint16_t selector;
	uint8_t paramcnt;
	uint8_t attribs;
	uint16_t offset16_31;
} idt_entry_t;

typedef struct idt_reg {
	uint16_t limit;
	uint8_t base[sizeof(uint32_t)];
} idt_reg_t;

typedef struct irq_context {
	uint32_t edi, esi, ebp, ebx, edx, ecx, eax;
	uint32_t es, ds;
	uint32_t irq;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
} __attribute__ ((packed)) irq_context_t;

void reprogram_pic();
void install_idt();
void end_of_irq(uint8_t irq);
void uninstall_handler(uint8_t irq);
void install_intr_handler(uint8_t irq, void *handler);
void install_trap_handler(uint8_t trap, void *handler);

#endif
