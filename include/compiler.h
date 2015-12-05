#ifndef __GCC_COMPILER_H
#define __GCC_COMPILER_H

#if ( __GNUC__ < 2 )
#error Your compiler is too old or not recognized!
#endif

/*
 * Initialization routine (the memory occupied by this routine will
 * be freed after the system initialization)
 */
#define __INIT__	__attribute__((__section__(".init")))

/*
 * Fast call routine.
 * warning: Do not use this for system calls
 */
#define FASTCALL( x )	__attribute__((__regparm__(3))) x

/*
 * The use of a deprecated routine will produces a warning message
 * during compilation time
 */
#define __DEPRECATED__	__attribute__((__deprecated__))

/* A routine declared inlined will be ever expanded by the compiler */
#define __INLINE__	__inline__ __attribute__((__always_inline__))

/* This routine can't be expanded by the compiler */
#define __NOINLINE__	__attribute__((__noinline__))

/* Procedure alignment to 16 byte */
#define __ALIGN16__	__attribute__((__aligned__(16)))

/* Procedure alignment to 32 byte */
#define __ALIGN32__	__attribute__((__aligned__(32)))

/* The function does not return */
#define __NORETURN__	__attribute__((__noreturn__))

#endif
