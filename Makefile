# Makefile for kernel tutorial
#
ifndef ARCH
ARCH = i386
endif

ifndef KERNELNAME
KERNELNAME = bagual
endif

ifndef QEMU
QEMU = $(shell if which qemu-system-${ARCH} &> /dev/null; \
        then echo qemu-system-${ARCH}; exit; \
        else \
        echo "***" 1>&2; \
        echo "*** Error: Couldn't find a working QEMU executable." 1>&2; \
        echo "*** Is the directory containing the qemu binary in your PATH" 1>&2; \
        echo "*** or have you tried setting the QEMU variable in Makefile?" 1>&2; \
        echo "***" 1>&2; exit 1; fi)
endif

CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)as
LD = $(TOOLPREFIX)ld
OBJDUMP = $(TOOLPREFIX)objdump

ARCHDIR = arch/$(ARCH)
LDARCH = $(shell $(LD) -V | grep elf_$(ARCH) | awk '{print $$1}')

CFLAGS = -m32 -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs \
	 -ffreestanding -fno-builtin -fno-stack-protector \
	 -Wall -Wextra -Werror -I $(ARCHDIR)/include/ -I include/

LDFLAGS = -m$(LDARCH) -g -nostdlib -nodefaultlibs

ASFLAGS = -m32 -g -Wa,-divide -I $(ARCHDIR)/include/ -I include/

OBJECTS = $(ARCHDIR)/boot.o $(ARCHDIR)/clock.o $(ARCHDIR)/exc.o \
	  $(ARCHDIR)/irq.o $(ARCHDIR)/exception.o $(ARCHDIR)/interrupt.o \
	  $(ARCHDIR)/keyboard.o $(ARCHDIR)/mem.o lib/stdlib.o lib/vsprintf.o \
	  lib/ctype.o kernel/main.o kernel/multiboot.o kernel/terminal.o

all: kernel

kernel: $(OBJECTS)
	$(LD) -T $(ARCHDIR)/linker.ld $(OBJECTS) $(LDFLAGS) -o $(ARCHDIR)/$@
	$(OBJDUMP) -t $(ARCHDIR)/$@ | \
	sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $@-$(ARCH).sym

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

%.o: %.S
	$(CC) -c $< -o $@ $(ASFLAGS)

clean:
	@rm -f $(OBJECTS) $(ARCHDIR)/kernel $(KERNELNAME).iso *.sym

iso: all
	@scripts/make-iso.sh $(KERNELNAME) $(ARCH)

$(KERNELNAME).iso: iso

test-run: $(KERNELNAME).iso
	$(QEMU) -cdrom $(KERNELNAME).iso
