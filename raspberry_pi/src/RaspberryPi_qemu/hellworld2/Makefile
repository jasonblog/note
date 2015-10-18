# Makefile - build script */
 
# build environment
PREFIX ?= /opt/arm-2008q3/
ARMGNU ?= $(PREFIX)/bin/arm-none-eabi
 
# source files
SOURCES_ASM := $(wildcard *.S)
SOURCES_C   := $(wildcard *.c)
 
# object files
OBJS        := $(patsubst %.S,%.o,$(SOURCES_ASM))
OBJS        += $(patsubst %.c,%.o,$(SOURCES_C))
 
# Build flags
DEPENDFLAGS := -MD -MP
INCLUDES    := -I include
BASEFLAGS   := -O2 -fpic -pedantic -pedantic-errors -nostdlib
BASEFLAGS   += -ffreestanding -mcpu=arm1176jzf-s
ASFLAGS     := $(INCLUDES) $(DEPENDFLAGS) -D__ASSEMBLY__
CFLAGS      := $(INCLUDES) $(DEPENDFLAGS) $(BASEFLAGS) $(WARNFLAGS)
CFLAGS      += -std=gnu99
 
# build rules
all: kernel.img
 
include $(wildcard *.d)
 
kernel.elf: $(OBJS) link-arm-eabi.ld
	$(ARMGNU)-ld $(OBJS) -Tlink-arm-eabi.ld -o $@
 
kernel.img: kernel.elf
	$(ARMGNU)-objcopy kernel.elf -O binary kernel.img
 
clean:
	$(RM) -f $(OBJS) kernel.elf kernel.img
 
distclean: clean
	$(RM) -f *.d
 
# C.
%.o: %.c Makefile
	$(ARMGNU)-gcc $(CFLAGS) -c $< -o $@
 
# AS.
%.o: %.S Makefile
	$(ARMGNU)-gcc $(ASFLAGS) -c $< -o $@
