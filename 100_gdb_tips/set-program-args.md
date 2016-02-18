# 設置被調試程序的參數

## 技巧

可以在gdb啟動時，通過選項指定被調試程序的參數，例如：

	$ gdb -args ./a.out a b c

也可以在gdb中，通過命令來設置，例如：

	(gdb) set args a b c
	(gdb) show args
	Argument list to give program being debugged when it is started is "a b c".

也可以在運行程序時，直接指定：

	(gdb) r a b
	Starting program: /home/xmj/tmp/a.out a b
	(gdb) show args
	Argument list to give program being debugged when it is started is "a b".
	(gdb) r
	Starting program: /home/xmj/tmp/a.out a b 

可以看出，參數已經被保存了，下次運行時直接運行`run`命令，即可。

有意的是，如果我接下來，想讓參數為空，該怎麼辦？是的，直接：

	(gdb) set args

詳情參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Arguments.html#Arguments)

## 貢獻者

xmj

