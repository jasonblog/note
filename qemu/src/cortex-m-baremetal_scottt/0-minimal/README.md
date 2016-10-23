Verify First Two Entries of the ISR Vector
$ export GDB=arm-none-eabi-gdb
$ gdbx minimal.elf 0 2a
0x0:	0x20010000	0x9 <Reset_Handler>
Analysis
0x20010000: initial stack pointer value. Typically the end of RAM.
0x09: a jump target with bit 0 set means Thumb mode. The code is really at 0x8
Disassemble Reset_Handler
$ gdbx minimal.elf 8 4i
   0x8 <Reset_Handler>:	ldr	r3, [pc, #4]	; (0x10 <Reset_Handler+8>)
   0xa <Reset_Handler+2>:	movs	r2, #17
   0xc <Reset_Handler+4>:	str	r2, [r3, #0]
   0xe <Reset_Handler+6>:	b.n	0xe <Reset_Handler+6>
Analysis
“b.n .”  infinite loop that jumps to itself. Equivalent to the for(;;) ; loop in C.
0x10 <Reset_Handler+8>: address of ‘x’ in literal pool
literal pool (aka. constant pool) is for constant loading
Look at The Address of Variable ‘x’
$ gdbx minimal.elf 0x10 1a
0x10 <Reset_Handler+8>:	0x20000000 <x>

Analysis
‘x’ is placed at the start of RAM
