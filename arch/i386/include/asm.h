/*
 * asm.h assembler macros to create x86 segments
 */
#ifndef ARCH_X86_ASM_H
#define ARCH_X86_ASM_H 1

#define SEG_NULLASM                                             \
        .word 0, 0;                                             \
        .byte 0, 0, 0, 0

/*
 *  The 0xC0 means the limit is in 4096-byte units
 *  and (for executable segments) 32-bit mode.
 */
#define SEG_ASM(type,base,lim)                                  \
        .word (((lim) >> 12) & 0xffff), ((base) & 0xffff);      \
        .byte (((base) >> 16) & 0xff), (0x90 | (type)),         \
                (0xC0 | (((lim) >> 28) & 0xf)), (((base) >> 24) & 0xff)

#define STA_X     0x8       /* Executable segment */
#define STA_E     0x4       /* Expand down (non-executable segments) */
#define STA_C     0x4       /* Conforming code segment (executable only) */
#define STA_W     0x2       /* Writeable (non-executable segments) */
#define STA_R     0x2       /* Readable (executable segments) */
#define STA_A     0x1       /* Accessed */

/* Control Register flags */
#define CR0_PE          0x00000001      // Protection Enable
#define CR0_MP          0x00000002      // Monitor coProcessor
#define CR0_EM          0x00000004      // Emulation
#define CR0_TS          0x00000008      // Task Switched
#define CR0_ET          0x00000010      // Extension Type
#define CR0_NE          0x00000020      // Numeric Errror
#define CR0_WP          0x00010000      // Write Protect
#define CR0_AM          0x00040000      // Alignment Mask
#define CR0_NW          0x20000000      // Not Writethrough
#define CR0_CD          0x40000000      // Cache Disable
#define CR0_PG          0x80000000      // Paging

#endif /* ARCH_X86_ASM_H */
