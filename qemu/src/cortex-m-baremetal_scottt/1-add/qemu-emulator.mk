.PHONY: emulator
emulator:
	qemu-system-arm -M lm3s6965evb -S -s -nographic -kernel /dev/null -semihosting
