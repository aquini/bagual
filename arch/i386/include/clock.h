#ifndef CLOCK_H
#define CLOCK_H

#include "times.h"
#include "interrupt.h"

#define LATCH_COUNT	0x00	 /* To copy chip count */
#define TIMER0		0x40	 /* I/O port for timer channel 0 */
#define TIMER2		0x42	 /* I/O port for timer channel 2 */
#define TIMER_MODE	0x43	 /* I/O port for timer mode control */
#define SQUARE_WAVE	0x36	 /* The sqare-wave form */
#define TIMER_FREQ	1193182L /* Clock frequency for timer in PC */

/* Clock Frequency (User settable, default=100Hz) */
#define HZ		100

/* Value to initialize timer */
#define TIMER_COUNT	(unsigned)(TIMER_FREQ/HZ)

/* LATCH is used for the interval timer */
#define LATCH		((TIMER_FREQ + HZ/2) / HZ)

/* Timeout variable structure */
typedef struct timeout {
	uint32_t start_ticks;
	uint32_t ticks_to_wait;
} timeout_t;

void init_clock();
void stop_clock();
unsigned long sys_get_ticks(void);
uint32_t sys_times(struct tms *buf);
void delay(uint32_t millisec);
void set_timeout(timeout_t *t, unsigned int millisec);
int is_timeout(timeout_t *t);
void clock_handler(irq_context_t *c);

#endif
