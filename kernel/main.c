#include "multiboot.h"
#include "terminal.h"
#include "keyboard.h"
#include "interrupt.h"
#include "types.h"
#include "clock.h"
#include "mem.h"

static inline void bochs_magic_break(void)
{
	asm volatile ("xchgw %bx, %bx");
}

void __NORETURN__  __NOINLINE__ scheduler(void)
{
	while (1) {
		rep_nop();
		barrier();
		safe_idle();
	}
}

extern void multiboot_checks(multiboot_info_t *mbi);
extern void panic(void);

void __NORETURN__ kernel_main(uint32_t magic, uint32_t addr)
{
	multiboot_info_t *mbi = (multiboot_info_t *)addr;

	terminal_initialize();
	cprintf("BAGUAL v0.0.1a\n");
	cprintf(" Kernel is running at address: 0x%x EIP: 0x%x\n",
		&kernel_main, get_eip());

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		cprintf("\n\n*** ERROR *** Invalid magic number: 0x%x\n", magic);
		panic();
	}

	cprintf("\n==[multiboot checks]=================\n");
	multiboot_checks(mbi);

	cprintf("\n==[System bootstrap]=================\n");
	cprintf("- Installing GDT ...\n");
	install_gdt();

	cprintf("- Arming PIC8259 ...\n");
	reprogram_pic();

	cprintf("- Installing IDT ...\n");
	install_idt();

	cprintf("- Initializing system clock ...\n");
	init_clock();

	cprintf("\nSystem ready...\n");
	init_keyboard();
	scheduler();
}
