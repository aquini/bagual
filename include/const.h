#ifndef CONST_H
#define CONST_H

#include "types.h"
#include "compiler.h"

#define TRUE	1
#define FALSE	0
#define NULL	0


#define MIN(a, b)	((a) < (b) ? (a) : (b))
#define MAX(a, b)	((a) > (b) ? (a) : (b))
#define ABS(a)		((a) < 0 ? -(a) : (a))
#define TRUNC(addr, align)	((addr) & ~((align) - 1))
#define ALIGN_DOWN(addr, align)	TRUNC(addr, align)
#define ALIGN_UP(addr, align)	( ((addr) + (align) - 1) & (~((align) - 1)) )

#endif
