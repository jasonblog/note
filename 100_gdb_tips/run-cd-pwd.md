# 在gdb中執行cd和pwd命令

## 技巧

是的，gdb確實支持這兩個命令，雖然我沒有想到它們有什麼特別的用處。

也許，當你啟動gdb之後，發現需要切換工作目錄，但又不想退出gdb的時候：

	(gdb) pwd
	Working directory /home/xmj.
	(gdb) cd tmp
	Working directory /home/xmj/tmp.

詳情參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Working-Directory.html#Working-Directory)

## 貢獻者

xmj

