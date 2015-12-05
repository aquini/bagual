#include "const.h"
#include "times.h"
#include "i386.h"
#include "interrupt.h"
#include "clock.h"
#include "terminal.h"

volatile unsigned long ticks = 0;

void stop_clock()
{
	uint32_t flags = local_irq_save();

	/* Set timer on */
	outb(TIMER_MODE, SQUARE_WAVE);
	/* Timer low byte */
	outb(TIMER0, 0);
	/* Timer high byte */
	outb(TIMER0, 0);

	local_irq_restore(flags);
}

void delay(uint32_t millisec)
{
	uint32_t count, elapsed;
	uint32_t flags = save_flags();

	elapsed = 0;
	for(;;) {
		if (elapsed >= millisec)
			break;

		count = ticks;
		enable_irqs();
		//schedule();
		while (count == ticks);
			rep_nop();

		elapsed += (ticks-count) * 1000 / HZ;
	}

	restore_flags(flags);
}

void set_timeout(timeout_t *t, unsigned int millisec)
{
	t->start_ticks = ticks;
	t->ticks_to_wait = millisec * HZ / 1000;
}

int is_timeout(timeout_t *t)
{
	register uint32_t elapsed = ticks - t->start_ticks;
	return elapsed >= t->ticks_to_wait;
}

unsigned long sys_get_ticks()
{
	return ticks;
}

void clock_handler(irq_context_t *c)
{
	ticks++;
	end_of_irq(c->irq);

	// Call the scheduler.
	//schedule();
}

void __INIT__ init_clock()
{
	uint32_t flags = local_irq_save();

	/* Set timer on */
	outb(TIMER_MODE, SQUARE_WAVE);
	/* Timer low byte */
	outb(TIMER0, TIMER_COUNT & 0xFF);
	/* Timer high byte */
	outb(TIMER0, TIMER_COUNT >> 8);

	local_irq_restore(flags);

	/* Install the clock handler */
	install_trap_handler(TIMER_IRQ, (void *)&clock_handler);
}
