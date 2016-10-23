#LINKER_SCRIPT := stm32f429zi_flash.ld
#LINKER_SCRIPT := tm4c123.ld
LINKER_SCRIPT := lm3s6965.ld

CFLAGS := -std=c11 -Wall -g3 -O3
CFLAGS += -mthumb -mcpu=cortex-m3
CFLAGS += -nostdlib
CXXFLAGS := $(CFLAGS)
LDFLAGS := 
EXECUTABLES := add.elf

include gcc-arm-embedded.mk

%.elf: %.c
	$(CC) $(CFLAGS) $+ $(LDFLAGS) -o $@

all: $(EXECUTABLES)

%.elf: LDFLAGS += -T$(LINKER_SCRIPT)

$(STARTUP_OBJ): $(STARTUP_CODE)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(EXECUTABLES) *.o core.*

.PHONY: all clean
.DELETE_ON_ERROR:

include qemu-emulator.mk

.PHONY: gdb-qemu
gdb-qemu: $(EXECUTABLES)
	@printf 'Run "make emulator" in another shell\n'
	$(GDB) -q -x add-qemu.gdb

.PHONY: gdb-observe-qemu
observe-qemu: $(EXECUTABLES)
	$(GDB) -q -x observe-qemu.gdb
