# 啟動時不顯示提示信息

## 例子

	$ gdb
	GNU gdb (GDB) 7.7.50.20140228-cvs
	Copyright (C) 2014 Free Software Foundation, Inc.
	License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
	This is free software: you are free to change and redistribute it.
	There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
	and "show warranty" for details.
	This GDB was configured as "x86_64-unknown-linux-gnu".
	Type "show configuration" for configuration details.
	For bug reporting instructions, please see:
	<http://www.gnu.org/software/gdb/bugs/>.
	Find the GDB manual and other documentation resources online at:
	<http://www.gnu.org/software/gdb/documentation/>.
	For help, type "help".
	Type "apropos word" to search for commands related to "word".

## 技巧
gdb在啟動時會顯示如上類似的提示信息。

如果不想顯示這個信息，則可以使用`-q`選項把提示信息關掉:

	$ gdb -q
	(gdb)

你可以在~/.bashrc中，為gdb設置一個別名：

	alias gdb="gdb -q"

詳情參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Invoking-GDB.html#Invoking-GDB)

## 貢獻者

xmj

