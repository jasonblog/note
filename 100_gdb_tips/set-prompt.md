# 設置命令提示符 

## 例子

	$ gdb -q `which gdb`
	Reading symbols from /home/xmj/install/binutils-gdb-git/bin/gdb...done.
	(gdb) r -q
	Starting program: /home/xmj/install/binutils-gdb-git/bin/gdb -q
	[Thread debugging using libthread_db enabled]
	Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".
	(gdb)

## 技巧

當你用gdb來調試gdb的時候，通過設置命令提示符，可以幫助你區分這兩個gdb：

	$ gdb -q `which gdb`
	Reading symbols from /home/xmj/install/binutils-gdb-git/bin/gdb...done.
	(gdb) set prompt (main gdb) 
	(main gdb) r -q
	Starting program: /home/xmj/install/binutils-gdb-git/bin/gdb -q
	[Thread debugging using libthread_db enabled]
	Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".
	(gdb) 

注意，這裡`set prompt (main gdb) `結尾處是有一個空格的。

詳情參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Prompt.html#Prompt)

## 貢獻者

xmj

