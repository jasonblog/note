# 在gdb中執行shell命令和make

## 技巧

你可以不離開gdb，直接執行shell命令，比如：

	(gdb) shell ls

或

	(gdb) !ls

這裡，"!"和命令之間不需要有空格（即，有也成）。

特別是當你在構建環境(build目錄)下調試程序的時候，可以直接運行make：

	(gdb) make CFLAGS="-g -O0"

詳情參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Shell-Commands.html#Shell-Commands)

## 貢獻者

xmj

