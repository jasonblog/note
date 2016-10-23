Read the Code
---
- Read [hello-semihosting.c](hello-semihosting.c)

Run the code
---

- Read [observe-qemu.gdb](observe-qemu.gdb)
- Run `make observe-qemu`

Observe at Assembly Level
---

- Read [hello-qemu.gdb](hello-qemu.gdb)
- Run `make gdb-qemu`
- Single step through the code and see that ARM semihosting calls are made with the `bkpt 0xab` instruction on ARMv7-M
