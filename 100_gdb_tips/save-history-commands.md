# 保存歷史命令

## 技巧

在gdb中，缺省是不保存歷史命令的。你可以通過如下命令來設置成保存歷史命令：

	(gdb) set history save on

但是，歷史命令是缺省保存在了當前目錄下的.gdb_history文件中。可以通過如下命令來設置要保存的文件名和路徑：

	(gdb) set history filename fname

現在，我們把這兩個命令放到$HOME/.gdbinit文件中：

	set history filename ~/.gdb_history
	set history save on

好了，下次啟動gdb時，你就可以直接查找使用之前的歷史命令了。

詳情參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Command-History.html#Command-History)

## 貢獻者

xmj

