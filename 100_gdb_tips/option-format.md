# 命令行選項的格式

## 技巧

gdb的幫助信息和在線文檔對於長選項的形式使用了不同的風格。你可能有點迷惑，gdb的長選項究竟應該是“-”，還是“--”？

是的，這兩種方式都可以。例如：

	$ gdb -help
	$ gdb --help

	$ gdb -args ./a.out a b c
	$ gdb --args ./a.out a b c

好吧，使用短的。

## 貢獻者

xmj

