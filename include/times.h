#ifndef TIMES_H
#define TIMES_H

#include "types.h"

struct tms
{
	uint32_t tms_utime;
	uint32_t tms_stime;
	uint32_t tms_cutime;
	uint32_t tms_cstime;
};

#endif
