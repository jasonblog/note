Read the Code
-------------

Read [add.c](add.c)

Verify that integer addition indeed works
-------

 - Read [observe-qemu.gdb](observe-qemu.gdb)
 - Run `make observe-qemu`
 - Check the output of GDB to see that integer addition indeed works

Observe how it works at the assembly language level
---------------------------------------------------

- Read [gdb-qemu.gdb](gdb-qemu.gdb)
- Run `make qemu-gdb`
- Use `stepi` to step through each instruction and observe how integer addition works at the ARMv7-M assembly level
