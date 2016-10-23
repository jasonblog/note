# Usage:
#
# CONFIG_NEWLIB_NANO := y
# CONFIG_NEWLIB_ARM_SEMIHOSTING := y
# CONFIG_NEWLIB_NANO_PRINTF_FLOAT := y
# include gcc-arm-embedded-newlib-nano.mk
# LDFLAGS += $(LIBC_LDFLAGS)

ifndef CONFIG_NEWLIB_NANO
CONFIG_NEWLIB_NANO := n
endif
ifndef CONFIG_NEWLIB_ARM_SEMIHOSTING
CONFIG_ARM_SEMIHOSTING := n
endif

# LIBC_LDFLAGS: linker flags for newlib-nano
# see gcc-arm-embedded: readme.txt
ifeq ($(CONFIG_NEWLIB_NANO),y)
LIBC_LDFLAGS += --specs=nano.specs

ifeq ($(CONFIG_NEWLIB_ARM_SEMIHOSTING),y)
# newlib syscall functions use semihosting
LIBC_LDFLAGS += --specs=rdimon.specs
else
# newlib syscall functions are no-ops
LIBC_LDFLAGS += --specs=nosys.specs
endif # CONFIG_NEWLIB_ARM_SEMIHOSTING

ifeq ($(CONFIG_NEWLIB_NANO_PRINTF_FLOAT),y)
LIBC_LDFLAGS += -u _printf_float
endif

ifeq ($(CONFIG_NEWLIB_NANO_SCANF_FLOAT),y)
LIBC_LDFLAGS += -u _scanf_float
endif

endif # CONFIG_NEWLIB_NANO
