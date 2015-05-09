# pstack 跟蹤進程棧
此命令可顯示每個進程的棧跟蹤。pstack 命令必須由相應進程的屬主或 root 運行。可以使用 pstack 來確定進程掛起的位置。此命令允許使用的唯一選項是要檢查的進程的 PID。請參見 proc(1) 手冊頁。

這個命令在排查進程問題時非常有用，比如我們發現一個服務一直處於work狀態（如假死狀態，好似死循環），使用這個命令就能輕鬆定位問題所在；可以在一段時間內，多執行幾次pstack，若發現代碼棧總是停在同一個位置，那個位置就需要重點關注，很可能就是出問題的地方；

示例：查看bash程序進程棧:
```
/opt/app/tdev1$ps -fe| grep bash
tdev1   7013  7012  0 19:42 pts/1    00:00:00 -bash
tdev1  11402 11401  0 20:31 pts/2    00:00:00 -bash
tdev1  11474 11402  0 20:32 pts/2    00:00:00 grep bash
/opt/app/tdev1$pstack 7013
#0  0x00000039958c5620 in __read_nocancel () from /lib64/libc.so.6
#1  0x000000000047dafe in rl_getc ()
#2  0x000000000047def6 in rl_read_key ()
#3  0x000000000046d0f5 in readline_internal_char ()
#4  0x000000000046d4e5 in readline ()
#5  0x00000000004213cf in ?? ()
#6  0x000000000041d685 in ?? ()
#7  0x000000000041e89e in ?? ()
#8  0x00000000004218dc in yyparse ()
#9  0x000000000041b507 in parse_command ()
#10 0x000000000041b5c6 in read_command ()
#11 0x000000000041b74e in reader_loop ()
#12 0x000000000041b2aa in main ()
```
