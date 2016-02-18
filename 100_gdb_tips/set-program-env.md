# 設置被調試程序的環境變量

## 例子

	(gdb) u 309
	Warning: couldn't activate thread debugging using libthread_db: Cannot find new threads: generic error
	Warning: couldn't activate thread debugging using libthread_db: Cannot find new threads: generic error
	warning: Unable to find libthread_db matching inferior's thread library, thread debugging will not be available.

## 技巧

在gdb中，可以通過命令`set env varname=value`來設置被調試程序的環境變量。對於上面的例子，網上可以搜到一些解決方法，其中一種方法就是設置LD_PRELOAD環境變量：

	set env LD_PRELOAD=/lib/x86_64-linux-gnu/libpthread.so.0

注意，這個實際路徑在不同的機器環境下可能不一樣。把這個命令加到~/.gdbinit文件中，就可以了。

詳情參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Environment.html#Environment)

## 貢獻者

xmj

