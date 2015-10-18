# KGDB  Linux Kernel for Samsung

##1 簡介

這個文檔記錄了用 kGDB 調試 Linux 內核的全過程，都是在前人工作基礎上的一些總結。以下操作都是基於特定板子來進行，但是大部分都能應用於其他平臺。

要使用 KGDB 來調試內核，首先需要修改 config 配置文件，打開相應的配置，配置內核啟動參數，甚至修改串口驅動添加 poll 支持，然後才能通過串口遠程調試內核。



## 2 配置內核

### 2.1 基本配置
在內核配置文件 .config 中，需要打開如下選項：

<table><tbody><tr><td>CONFIG_KGDB</td><td>加入KGDB支持</td></tr><tr><td>CONFIG_KGDB_SERIAL_CONSOLE</td><td>使KGDB通過串口與主機通信(打開這個選項，默認會打開CONFIG_CONSOLE_POLL和CONFIG_MAGIC_SYSRQ)</td></tr><tr><td>CONFIG_KGDB_KDB</td><td>加入KDB支持</td></tr><tr><td>CONFIG_DEBUG_KERNEL</td><td>包含驅動調試信息</td></tr><tr><td>CONFIG_DEBUG_INFO</td><td>使內核包含基本調試信息</td></tr><tr><td>CONFIG_DEBUG_RODATA=n</td><td>關閉這個，能在只讀區域設置斷點
</td></tr></tbody></table>

### 2.2 可選選項

<table><tbody><tr><td>CONFIG_PANIC_TIMEOUT=5</td><td>&nbsp;</td></tr><tr><td>CONFIG_BOOTPARAM_SOFTLOCKUP_PANIC_VALUE=1</td><td>&nbsp;</td></tr><tr><td>CONFIG_BOOTPARAM_HUNG_TASK_PANIC_VALUE=1</td><td>&nbsp;</td></tr><tr><td>CONFIG_S3C2410_WATCHDOG_ATBOOT=0</td><td>&nbsp;</td></tr><tr><td>CONFIG_FRAME_POINTER</td><td>使KDB能夠打印更多的棧信息</td></tr><tr><td>CONFIG_KALLSYMS</td><td>加入符號信息</td></tr><tr><td>CONFIG_KDB_KEYBOARD</td><td>如果是通過目標版的鍵盤與KDB通信，需要把這個打開，且鍵盤不能是USB接口</td></tr><tr><td>CONFIG_KGDB_TESTS</td><td>&nbsp;
</td></tr></tbody></table>


### 2.3 啟動參數

打開相應的選項後，需要配置 kernel 啟動參數，使 KGDB 和內核能夠找到正確的通信接口。如果是使用串口，則需要配置如下選項：

```
console=ttySAC3,115200 kgdboc=ttySAC3,115200
```
如果需要調試內核的啟動過程，則需要在 kgdboc 後面加入 kgdbwait 。

在其他板子上，若使用以太網口來和 KGDB 進行通信，則要把 kgdboc 換成 kgdboe(kgdb over ethernet) ）。

配置完後，就可以正常編譯，然後把內核下載到目標板上面。

## 3 串口驅動修改

如果在內核啟動的過程中出現如下錯誤提示：
```
kgdb: Unregistered I/O driver, debugger disabled.
```

則需要根據這一部分，修改串口驅動程序，若能正常進入 kgdb ，則忽略該節，直接進入下一節使用 KGDB 。

在 `drivers/tty/serial/kgdboc.c` 中的 `configure_kgdboc` 函數，會通過 `tty_find_polling_driver(cptr, &tty_line)` 來找尋內核啟動參數中指定的串口驅動。然後通過 `kgdboc_get_char() `和 `kgdboc_put_char()` 來和主機串口正常通信。

可以看到在 config 配置文件的` CONFIG_CONSOLE_POLL `就是使能串口與 kgdboc 的接口。如果` tty_find_polling_driver `沒有找到對應的串口通信接口，則會調用 `kernel/debug/debug_core.c `中的` kgdb_unregister_io_module `進行錯誤處理。

有的板子的串口驅動並沒有加入對 kgdboc 通信的支持，例如 Samsung 的串口驅動需要在 `drivers/tty/serial/samsung.c `中手動添加。 添加與 kgdboc 通信的接口，只需添加一個發送函數和接收函數，然後在驅動操作結構體中加入對應的函數就可以了。具體的 PATCH 如下：

```c
drivers/tty/serial/samsung.c | 22 ++++++++++++++++++++++
1 files changed, 22 insertions(+), 0 deletions(-)
diff --git a/drivers/tty/serial/samsung.c b/drivers/tty/serial/samsung.c
index ff6a4f8..5ceb7d7 100755
--- a/drivers/tty/serial/samsung.c
+++ b/drivers/tty/serial/samsung.c
@@ -893,7 +893,29 @@ static struct console s3c24xx_serial_console;
#define S3C24XX_SERIAL_CONSOLE NULL
#endif
+#ifdef CONFIG_CONSOLE_POLL
+static void s3c24xx_serial_poll_put_char(struct uart_port *port, unsigned char c)
+{
+    while (!(rd_regl(port, S3C2410_UTRSTAT) & S3C2410_UTRSTAT_TXE))
+       ;
+
+    wr_regl(port, S3C2410_UTXH, c);
+}
+
+static int s3c24xx_serial_poll_get_char(struct uart_port *port)
+{
+    while (!(rd_regl(port, S3C2410_UTRSTAT) & S3C2410_UTRSTAT_RXDR))
+        ;
+
+    return rd_regl(port, S3C2410_URXH);
+}
+#endif
+
static struct uart_ops s3c24xx_serial_ops = {
+#ifdef CONFIG_CONSOLE_POLL
+    .poll_get_char = s3c24xx_serial_poll_get_char,
+    .poll_put_char = s3c24xx_serial_poll_put_char,
+#endif
     .pm = s3c24xx_serial_pm,
     .tx_empty = s3c24xx_serial_tx_empty,
     .get_mctrl = s3c24xx_serial_get_mctrl,
--
1.7.5.4
```
加入這個 patch ，重新編譯內核，之後就能正常進入 kgdb

## 4 gdb 遠程調試
如果在內核啟動參數中加入了 kgdbwait ，則內核會在完成基本的初始化之後，停留在 kgdb 的調試陷阱中，等待主機的 gdb 的遠程連接。

由於大部分的板子只有一個調試串口，所以你需要把之前與串口通信的 minicom 退出來，然後在內核源碼的目錄下，執行以下命令：

```sh
$ arm-linux-gnueabi-gcc vmlinux
(gdb) target remote /dev/ttyUSB0
(gdb) set detach-on-fork on
(gdb) b panic()
(gdb) c
```

當然，你也可以 agent-proxy 來復用一個串口，通過虛擬出兩個 TCP 端口。這時候， gdb 就需要用 target remote 命令連接 kgdb ，例如：

```sh
(gdb) target remote localhost:5551
```
agent-proxy 可這樣下載：

```sh
git clone git://git.kernel.org/pub/scm/utils/kernel/kgdb/agent-proxy.git
```

具體用法，請看該 repo 下的 README 。

在用 gdb 來調試內核的時候，由於內核在初始化的時候，會創建很多子線程。而默認 gdb 會接管所有的線程，如果你從一個線程切換到另外一個線程， gdb 會馬上把原先的線程暫停。但是這樣`很容易導致 kernel 死掉`，所以需要設置一下 gdb 。一般用 gdb 進行多線程調試，需要注意兩個參數：
`follow-fork-mode` 和 `detach-on-fork`。


- detach-on-fork 參數，指示 GDB 在 fork 之後是否斷開（detach）某個進程的調試，或者都交由 GDB 控制： `set detach-on-fork [on|off]`
    - on: 斷開調試 follow-fork-mode 指定的進程。
    - off: gdb將控制父進程和子進程。

- follow-fork-mode 指定的進程將被調試，另一個進程置於暫停（suspended）狀態。follow-fork-mode 的用法為：`set follow-fork-mode [parent|child]`
    - parent: fork之後繼續調試父進程，子進程不受影響。
    - child: fork之後調試子進程，父進程不受影響。


##5 參考資料
```
gdb user mannual
gdb internal
kgdb/kdb official website
kernel debug usage
kdb in elinux.org
multi-threads debug in gdb
KGDB.info
```
