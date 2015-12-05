#include "const.h"
#include "i386.h"
#include "mem.h"
#include "interrupt.h"
#include "terminal.h"

#define GDT_DOUBLE_FAULT_ENTRY  0x28
#define KERNEL_CODE             0x10


volatile idt_entry_t idt[IDT_SIZE];
volatile idt_reg_t idt_ptr;

/*
 * Extern IRQ entry points (defined inside irq.s)
 */
extern int _irq_00, _irq_01, _irq_02, _irq_03, _irq_04, _irq_05, _irq_06, _irq_07;
extern int _irq_08, _irq_09, _irq_0A, _irq_0B, _irq_0C, _irq_0D, _irq_0E, _irq_0F;
extern int _irq_unhand;

/*
 * Extern EXC entry points (defined inside exc.s)
 */
extern int _exc_00, _exc_01, _exc_02, _exc_03, _exc_04, _exc_05, _exc_06, _exc_07;
extern int _exc_09, _exc_0A, _exc_0B, _exc_0C, _exc_0D, _exc_0E, _exc_0F;
extern int _exc_unhand;

void unhandled_interrupt(irq_context_t *ctx);

typedef struct irq_desc {
	void (*handler)(irq_context_t *ctx);
} irq_desc_t;

irq_desc_t irq_handler[IRQ_NR] __attribute__ ((aligned(16))) =
{
	[0 ... IRQ_NR-1] = { .handler = unhandled_interrupt  }
};

/* PIC irq mask */
uint16_t irq_mask = 0xffff;

/* PIC irq mask for the master controller */
#define irq_master_mask		(((unsigned char *)&(irq_mask))[0])

/* PIC irq mask for the slave controller */
#define irq_slave_mask		(((unsigned char *)&(irq_mask))[1])

static inline void enable_irq(uint8_t irq)
{
	unsigned int mask = ~(1 << irq);
	uint32_t flags = local_irq_save();

	irq_mask &= mask;
	if (irq & 8)
		outb(PORT_INT_MASK_S, irq_slave_mask);
	else
		outb(PORT_INT_MASK_M, irq_master_mask);

	local_irq_restore(flags);
}

static inline void disable_irq(uint8_t irq)
{
	unsigned int mask = 1 << irq;
	uint32_t flags = local_irq_save();

	irq_mask |= mask;
	if (irq & 8)
		outb(PORT_INT_MASK_S, irq_slave_mask);
	else
		outb(PORT_INT_MASK_M, irq_master_mask);

	local_irq_restore(flags);
}

void __INIT__ reprogram_pic(void)
{
	/* Start initialization for master 8259a controller */
	outb(PORT_8259_M, 0x11);
	/* Master base vector [ 0x20 ... 0x27 ] */
	outb(PORT_INT_MASK_M, 0x20);
	/* IRQ2 cascade to slave */
	outb(PORT_INT_MASK_M, 1 << 2);
	/* Finish master 8259a initialization (manual EOI) */
	outb(PORT_INT_MASK_M, 0x01);

	/* Start initialization for slave 8259a controller */
	outb(PORT_8259_S, 0x11);
	/* Slave base vector [ 0x28 ... 0x2f ] */
	outb(PORT_INT_MASK_S, 0x28);
	/* Cascade on IRQ2 */
	outb(PORT_INT_MASK_S, 2);
	/* Finish slave 8259a initialization */
	outb(PORT_INT_MASK_S, 0x01);

	/* Initialize the irq mask for master 8259a */
	irq_mask &= ~(1 << 2);
	outb(PORT_INT_MASK_M, irq_master_mask);

	/* Initialize the irq mask for master 8259a */
	outb(PORT_INT_MASK_S, irq_slave_mask);
}

void uninstall_handler(uint8_t irq)
{
	uint32_t flags;

	if (irq > IRQ_NR)
		return;

	flags = local_irq_save();
	irq_handler[irq].handler = NULL;
	disable_irq(irq);
	local_irq_restore(flags);
	return;
}

void install_intr_handler(uint8_t irq, void *handler)
{
	uint32_t flags;

	if (irq > IRQ_NR)
		return;

	flags = local_irq_save();
	irq_handler[irq].handler = handler;
	idt[0x20 + irq].attribs = INT_GATE;
	enable_irq(irq);
	local_irq_restore(flags);
	return;
}

void install_trap_handler(uint8_t irq, void *handler)
{
	uint32_t flags;

	if (irq > IRQ_NR)
		return;

	flags = local_irq_save();
	irq_handler[irq].handler = handler;
	idt[0x20 + irq].attribs = TRAP_GATE;
	enable_irq(irq);
	local_irq_restore(flags);
	return;
}

void unhandled_interrupt(irq_context_t *c)
{
	uint32_t flags = local_irq_save();
	cprintf("%s(): unexpected IRQ number %03u!\n",
		__FUNCTION__, c->irq);
	local_irq_restore(flags);
}

inline void end_of_irq(uint8_t irq)
{
	/* Simply re-enable the irq line */
	enable_irq(irq);
}

/* Check if the IRQ occurred can be considered spurious or real */
static inline int irq_real(uint8_t irq)
{
	int value;
	int irqmask = 1 << irq;

	if (irq < 8) {
		outb(PORT_8259_M, 0x0B);
		value = inb(PORT_8259_M) & irqmask;
		outb(PORT_8259_M, 0x0A);
		return value;
	}
	outb(PORT_8259_S, 0x0B);
	value = inb(PORT_8259_S) & (irqmask >> 8);
	outb(PORT_8259_S, 0x0A);
	return value;
}

/* Acknowledge the irq line */
static inline void mask_and_ack_irq(int irq)
{
	unsigned int mask = 1 << irq;
	uint32_t flags = local_irq_save();

	/*
	 * Spurious IRQ detection. Remember that spurious IRQs are usually sign
	 * of hardware problems, so we want to do the strictly needed checks
	 * without slowing down good hardware unnecessarily.
	 *
	 * Note: IRQ7 and IRQ15 (the two spurious IRQs usually resulting from
	 * the 8259A-1|2 PICs) seems to occur even if the IRQ is masked in the
	 * 8259A.
	 */
	if (irq_mask & mask)
		goto spurious_irq;

	/* Mask the irq line */
	irq_mask |= mask;

handle_real_irq:
	if (irq & 8) {
		/* Dummy operation (required) */
		(void)inb(PORT_INT_MASK_S);

		/* Mask the irq on the slave */
		outb(PORT_INT_MASK_S, irq_slave_mask);
		outb(PORT_8259_S, 0x60 + (irq & 7));
		outb(PORT_8259_S, 0x60 + 2);

		/* Acknowledge the irq sending a EOI */
		outb(PORT_8259_S, EOI);
		outb(PORT_8259_M, EOI);
	} else {
		/* Dummy operation (but required) */
		(void)inb(PORT_INT_MASK_M);

		/* Mask the irq on the master */
		outb(PORT_INT_MASK_M, irq_master_mask);
		outb(PORT_8259_M, 0x60 + irq);

		/* Acknowledge the irq sending a EOI */
		outb(PORT_8259_M, EOI);
	}

	local_irq_restore(flags);
	return;

spurious_irq:
	if (irq_real(irq))
		goto handle_real_irq;

        {
		static int spurious_irq_mask;
		/*
		 * At this point we can be sure the IRQ is spurious,
		 * lets ACK and report it. [once per IRQ]
		 */
		if (!(spurious_irq_mask & mask)) {
			cprintf("spurious 8259A interrupt: IRQ%d.\n", irq);
			spurious_irq_mask |= mask;
		}
		/*
		 * Theoretically we do not have to handle this IRQ,
		 * but in Linux this does not cause problems and is
		 * simpler for us.
		 */
		goto handle_real_irq;
	}

}

void default_handler(irq_context_t *ctx)
{
	/* Mask and acknowledge the irq line */
	mask_and_ack_irq(ctx->irq);

	/* Handle the IRQ */
	if (ctx->irq < IRQ_NR ) {
		irq_handler[ctx->irq].handler(ctx);
	} else {
		unhandled_interrupt(ctx);
	}

	/* Re-enable the irq line */
	end_of_irq(ctx->irq);
}

void setup_idt_entry(uint8_t i, uint16_t selector, uint32_t offset,
		     uint8_t attribs, uint8_t paramcnt)
{
	uint32_t flags = local_irq_save();

	idt[i].offset0_15 =  offset & 0xFFFF;
	idt[i].offset16_31 = offset >> 16;
	idt[i].selector = selector;
	idt[i].attribs = attribs;
	idt[i].paramcnt = paramcnt;

	local_irq_restore(flags);
}

static inline void set_intr_gate(unsigned int n, void *addr)
{
	setup_idt_entry(n, KERNEL_CODE, (uint32_t)addr, INT_GATE, 0);
}

static inline void set_trap_gate(unsigned int n, void *addr)
{
	setup_idt_entry(n, KERNEL_CODE, (uint32_t)addr, TRAP_GATE, 0);
}

void __INIT__ install_idt()
{
	unsigned int i;

	/* Initialize exception handlers (0x00..0x1F) */
	for (i = 0x00; i <= 0x1F; i++)
		set_intr_gate(i, &_exc_unhand);

	/* Setup the IDT entries for exceptions */

	set_trap_gate(0x00, &_exc_00); /* divide error */
	set_trap_gate(0x01, &_exc_01); /* debug */
	set_intr_gate(0x02, &_exc_02); /* nmi */
	set_trap_gate(0x06, &_exc_06); /* invalid operation */
	set_trap_gate(0x07, &_exc_07); /* device not available */
	set_trap_gate(0x09, &_exc_09); /* coprocessor segment overrun */
	set_trap_gate(0x0a, &_exc_0A); /* invalid tss */
	set_trap_gate(0x0b, &_exc_0B); /* segment not present */
	set_trap_gate(0x0c, &_exc_0C); /* stack fault */
	set_trap_gate(0x0d, &_exc_0D); /* general protection */
	set_intr_gate(0x0e, &_exc_0E); /* page fault */
	set_intr_gate(0x0f, &_exc_0F); /* spurious interrupt */

	/* Initialize interrupt handlers 0x20..0xFF */
	for (i = 0x20; i <= 0xFF; i++)
		set_intr_gate(i, &_irq_unhand);

	/* Setup the IDT entries for IRQs */
	set_trap_gate(0x20, &_irq_00);
	set_trap_gate(0x21, &_irq_01);
	set_trap_gate(0x22, &_irq_02);
	set_trap_gate(0x23, &_irq_03);
	set_trap_gate(0x24, &_irq_04);
	set_trap_gate(0x25, &_irq_05);
	set_trap_gate(0x26, &_irq_06);
	set_trap_gate(0x27, &_irq_07);
	set_trap_gate(0x28, &_irq_08);
	set_trap_gate(0x29, &_irq_09);
	set_trap_gate(0x2a, &_irq_0A);
	set_trap_gate(0x2b, &_irq_0B);
	set_trap_gate(0x2c, &_irq_0C);
	set_trap_gate(0x2d, &_irq_0D);
	set_trap_gate(0x2e, &_irq_0E);
	set_trap_gate(0x2f, &_irq_0F);

	/* Set up the IDT pointer */
	idt_ptr.limit = (IDT_SIZE * sizeof(idt_entry_t) - 1);
	*(size_t *)idt_ptr.base = ((size_t)&idt);

	/* Load info into IDTR register */
	__asm__ __volatile__ ("lidtl (%0)" : : "r"((uint32_t)&idt_ptr));

	/* Enable interrupts */
	enable_irqs();
}
