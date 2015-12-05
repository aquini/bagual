/*
 * For a complete documentation about GDT and descriptors
 * you can see the "IA-32 Intel® Architecture Software Developer's
 * Manual - Volume 3: System Programming Guide"
 */

#include "const.h"
#include "i386.h"
#include "terminal.h"
#include "mem.h"
#include "stdlib.h"

volatile gdt_entry_t gdt[GDT_DIM];
static gdt_reg gdt_ptr;

uint16_t setup_gdt_entry(uint16_t limit, uint32_t base,
			 uint8_t attribs0_7, uint8_t attribs8_15)
{
	register unsigned short i;
	uint32_t flags = local_irq_save();

	for (i = 1; i < GDT_DIM; i++) {
		if (!(gdt[i].attribs0_7)) {
			gdt[i].limit = limit;
			gdt[i].base0_15 = base & 0xFFFF;
			gdt[i].base16_23 = (base >> 16) & 0xFF;
			gdt[i].base24_31 = (base >> 24) & 0xFF;
			gdt[i].attribs0_7 = attribs0_7;
			gdt[i].attribs8_15 = attribs8_15;
			local_irq_restore(flags);
			return (i * GDT_ENTRY_DIM);
		}
	}
	local_irq_restore(flags);
	return NULL;
}

void remove_gdt_entry(uint16_t sel)
{
	uint32_t flags = local_irq_save();

	gdt[sel/GDT_ENTRY_DIM].limit = 0;
	gdt[sel/GDT_ENTRY_DIM].base0_15 = 0;
	gdt[sel/GDT_ENTRY_DIM].base16_23 = 0;
	gdt[sel/GDT_ENTRY_DIM].base24_31 = 0;
	gdt[sel/GDT_ENTRY_DIM].attribs0_7 = 0;
	gdt[sel/GDT_ENTRY_DIM].attribs8_15 = 0;

	local_irq_restore(flags);
}

void __INIT__ install_gdt()
{
	/* First of all reset all entries in the GDT */
	memset((void *)&gdt, 0, sizeof(gdt));


	/* Dummy descriptor (required) */
	setup_gdt_entry(0, 0, 0, 0);
	/* KERNEL_DATA descriptor 0x08 */
	setup_gdt_entry(0xFFFF, 0, DATA_SEG, (ATTR_GRANUL | ATTR_32BIT | 0xF));
	/* KERNEL_CODE descriptor 0x10 */
	setup_gdt_entry(0xFFFF, 0, CODE_SEG, (ATTR_GRANUL | ATTR_32BIT | 0xF));
	/* USER_DATA descriptor 0x18 */
	setup_gdt_entry(0xFFFF, 0, (DATA_SEG | DPL_3), (ATTR_GRANUL | ATTR_32BIT | 0xF));
	/* USER_CODE descriptor 0x20 */
	setup_gdt_entry(0xFFFF, 0, (CODE_SEG | DPL_3), (ATTR_GRANUL | ATTR_32BIT | 0xF));

	/* Set up the GDT pointer */
	gdt_ptr.limit = (uint16_t)(GDT_DIM * GDT_ENTRY_DIM - 1);
	gdt_ptr.base = (size_t)gdt;

	/* Load info into GDTR */
	__asm__ __volatile__ ("lgdtl (%0)" : : "r"(&gdt_ptr));

	/* Update segment registers */
	k_update_segment_regs();
}
