#ifndef __I386_H
#define __I386_H

#include "const.h"
#include "types.h"

static inline void clts()
{
	__asm__ __volatile__ ("clts");
}

static inline void slts()
{
	unsigned int __cr0;
	__asm__ __volatile__ (
		"mov %%cr0, %0\n"
		"or $8, %0\n"
		"mov %0, %%cr0" : "=r"(__cr0));
}

static inline uint32_t rdtsc(void)
{
	uint32_t __h, __l;
	__asm__ __volatile__ ("rdtsc" : "=a"(__l), "=d"(__h));
	return (__h | __l);
}

static inline uint64_t rdtscl(void)
{
	uint64_t __v;
	__asm__ __volatile__ ("rdtsc" : "=A"(__v));
	return __v;
}

static inline void barrier(void)
{
	__asm__ __volatile__ ("lock; addl $0,0(%%esp)": : :"memory");
}

static inline void nop(void)
{
	__asm__ __volatile__ ("nop");
}

static inline void* get_eip(void)
{
	void *eip;
	__asm__ __volatile__ ("mov $1f,%0\n1:" : "=g" (eip));
	return eip;
}

static inline void enable_irqs(void)
{
	__asm__ __volatile__ ("sti" : : : "memory");
}

static inline void disable_irqs(void)
{
	__asm__ __volatile__ ("cli" : : : "memory");
}

static inline uint32_t save_flags(void)
{
	uint32_t __flags;
	__asm__ __volatile__ ("pushfl ; popl %0" : "=g"(__flags) :);
	return __flags;
}

static inline void restore_flags(uint32_t flags)
{
	__asm__ __volatile__ (
		"pushl %0 ; popfl" : : "g"(flags) : "memory", "cc");
}

static inline uint32_t local_irq_save(void)
{
	uint32_t __flags = save_flags();
	disable_irqs();
	return __flags;
}

static inline uint32_t local_irq_set(void)
{
	uint32_t __flags = save_flags();
	enable_irqs();
	return __flags;
}

static inline void local_irq_restore(uint32_t flags)
{
	restore_flags(flags);
}

static inline void idle(void)
{
	__asm__ __volatile__ ("hlt" : : : "memory");
}

static inline void safe_idle(void)
{
	do {
		enable_irqs();
		idle();
	} while (0);
}

static inline uint8_t inb(uint16_t port)
{
	register uint8_t val;
	__asm__ __volatile__ ("inb %%dx, %%al" : "=a" (val) : "d" (port));
        return( val );
}

static inline uint16_t inw(uint16_t port)
{
	register uint16_t val;
	__asm__ __volatile__ ("inw %%dx, %%ax" : "=a" (val) : "d" (port));
        return( val );
}

static inline uint32_t inl(uint16_t port)
{
	register uint32_t val;
	__asm__ __volatile__ ("inl %%dx, %%eax" : "=a" (val) : "d" (port));
        return( val );
}

static inline void outb(uint16_t port, uint8_t val)
{
	__asm__ __volatile__ ("outb %%al, %%dx" : : "d" (port), "a" (val));
}

static inline void outw(uint16_t port, uint16_t val)
{
	__asm__ __volatile__ ("outw %%ax, %%dx" : : "d" (port), "a" (val));
}

static inline void outl(uint16_t port, uint32_t val)
{
	__asm__ __volatile__ ("outl %%eax, %%dx" : : "d" (port), "a" (val));
}

/*
 * Copy a sequence of 'count' bytes to the buffer 'addr' from
 * the I/O port 'port'
 */
static inline void insb(unsigned short port, void * addr, unsigned long count)
{
	__asm__ __volatile__ (
		"rep ; insb":
		"=D"(addr), "=c"(count) : "d"(port), "0"(addr), "1"(count));
}

/*
 * Copy a sequence of 'count' words to the buffer 'addr' from
 * the I/O port 'port'
 */
static inline void insw(unsigned short port, void * addr, unsigned long count)
{
	__asm__ __volatile__ (
		"rep ; insw":
		"=D"(addr), "=c"(count) : "d"(port), "0"(addr), "1"(count));
}

/*
 * Copy a sequence of 'count' double words to the buffer 'addr'
 * from the I/O port 'port'
 */
static inline void insl(unsigned short port, void * addr, unsigned long count)
{
	__asm__ __volatile__ (
		"rep ; insl":
		"=D"(addr), "=c"(count) : "d"(port), "0"(addr), "1"(count));
}

/*
 * Copy a sequence of 'count' bytes to the I/O port 'port'
 * from the buffer 'addr'
 */
static inline void outsb(unsigned short port, void * addr, unsigned long count)
{
	__asm__ __volatile__ (
		"rep ; outsb":
		"=S"(addr), "=c"(count) : "d"(port), "0"(addr), "1"(count));
}

/*
 * Copy a sequence of 'count' words to the I/O port 'port'
 * from the buffer 'addr'
 */
static inline void outsw(unsigned short port, void * addr, unsigned long count)
{
	__asm__ __volatile__ (
		"rep ; outsw":
		"=S"(addr), "=c"(count) : "d"(port), "0"(addr), "1"(count));
}

/*
 * Copy a sequence of 'count' double words to the I/O port 'port'
 * from the buffer 'addr'
 */
static inline void outsl(unsigned short port, void * addr, unsigned long count)
{
	__asm__ __volatile__ (
		"rep ; outsl":
		"=S"(addr), "=c"(count) : "d"(port), "0"(addr), "1"(count));
}

/* This is truly the end. You have to reboot the system	after this... */
static inline void halt()
{
	__asm__ __volatile__ ("cli\n hlt");
}

/* Pause - this is a good thing to insert into busy-wait loops. */
static inline void rep_nop(void)
{
	__asm__ __volatile__ ("rep; nop" : : : "memory");
}

#endif
