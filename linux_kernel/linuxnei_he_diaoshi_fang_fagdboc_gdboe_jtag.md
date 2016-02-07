# Linux內核調試方法 gdboc gdboe JTAG


1. 非嵌入式應用程序開發：
編譯時加上調試信息, 在Makefile裡加上 CFLAGS += -g, 使用gdb（文本界面）調試， 或者境加型cgdb， 圖形界面有DDD等等。

2. 嵌入式應用程序開發：
編譯時加上調試信息, 在Makefile裡加上 CFLAGS += -g, 在開發機上運行gdb通過網絡連接到目標機，目標機運行gdbserver.

3. Linux內核調試方法：
源代碼級調試：kgdb，kgdb調試需要在make menuconfig內核選項中選上gdboc(串口），或者gdboe(以太網）和相應在的選項。kgdboc需要特別的串口驅動程序，例如8250。

源代碼級調試：JTAG調試，需要硬件「仿真器」，例如Jlink, BDI3000等。

我的經驗：gdb, gdbserver, Jlink都使用過。Jlink調試時，在開發機windows平臺上運行「Jlink自帶的gdb服務器」，通過Linux的gdb網絡連接到「Jlink自帶的gdb服務器」。kgdbc在Mini2440上不成功, 因為需要特別的串口驅動。