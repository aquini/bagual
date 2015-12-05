#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "const.h"

//! Context after an exception.
typedef struct exc_context {
	uint32_t edi, esi, ebp, ebx, edx, ecx, eax;
	uint32_t es, ds;
	uint32_t exc, err_code;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
} __attribute__ ((packed)) exc_context_t;

enum exception_codes {
	EXC_ZERO_DIV = 0x00,
	EXC_DEBUG = 0x01,
	EXC_NMI = 0x02,
	EXC_BREAKPNT = 0x03,
	EXC_OVERFLOW = 0x04,
	EXC_BOUND = 0x05,
	EXC_INV_OP_CODE = 0x06,
	EXC_PROC_EXT = 0x07,
	EXC_DOUBLE_FAULT = 0x08,
	EXC_PROC_EXT_FAULT = 0x09,
	EXC_INVALID_TSS = 0x0A,
	EXC_SEGMENT_INV = 0x0B,
	EXC_STACK_FAULT = 0x0C,
	EXC_GEN_PROT_FAULT = 0x0D,
	EXC_PAGE_FAULT = 0x0E,
	EXC_UNKNOWN = 0x0F,
	EXCEPTIONS
};

const char *exception_string[] = {
	"DIVISION BY ZERO",
	"DEBUG EXCEPTION DETECTED",
	"NON MASKABLE INTERRUPT",
	"BREAKPOINT INSTRUCTION DETECTED",
	"INTO DETECTED OVERFLOW",
	"BOUND RANGE EXCEEDED",
	"INVALID OPCODE",
	"PROCESSOR EXTENSION NOT AVAILABLE",
	"DOUBLE FAULT DETECTED",
	"PROCESSOR EXTENSION PROTECTION FAULT",
	"INVALID TASK STATE SEGMENT",
	"SEGMENT NOT PRESENT",
	"STACK FAULT",
	"GENERAL PROTECTION FAULT",
	"PAGE FAULT (protection fault)!",
	"UNKNOWN EXCEPTION",
	""
};

#endif
