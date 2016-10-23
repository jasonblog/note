#LINKER_SCRIPT := stm32f429zi_flash.ld
#LINKER_SCRIPT := tm4c123.ld
LINKER_SCRIPT := lm3s6965.ld

CFLAGS := -Wall -g3 -O3
CFLAGS += -mthumb -mcpu=cortex-m3
CFLAGS += -nostdlib
CXXFLAGS := $(CFLAGS)
LDFLAGS := 
PROJECT := hello-semihosting
EXECUTABLES := $(PROJECT).elf

include gcc-arm-embedded.mk
GDB := $(HOST)-gdb-py

$(PROJECT).elf: startup.c $(PROJECT).c
	$(CC) $(CFLAGS) $+ $(LDFLAGS) -o $@

all: $(EXECUTABLES)

%.elf: LDFLAGS += -T$(LINKER_SCRIPT)

$(STARTUP_OBJ): $(STARTUP_CODE)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(EXECUTABLES) core.*

.PHONY: all clean
.DELETE_ON_ERROR:

include qemu-emulator.mk

.PHONY: gdb-qemu
gdb-qemu: $(EXECUTABLES)
	$(GDB) -q -x hello-qemu.gdb
