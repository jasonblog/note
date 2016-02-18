# 使用命令的縮寫形式

## 技巧

在gdb中，你不用必須輸入完整的命令，只需命令的（前）幾個字母即可。規則是，只要這個縮寫不會和其它命令有歧義（注，是否有歧義，這個規則從文檔上看不出，看起來需要查看gdb的源代碼，或者在實際使用中進行總結）。也可以使用tab鍵進行命令補全。

其中許多常用命令只使用第一個字母就可以，比如：

	b -> break
	c -> continue
	d -> delete
	f -> frame
	i -> info
	j -> jump
	l -> list
	n -> next
	p -> print
	r -> run
	s -> step
	u -> until

也有使用兩個或幾個字母的，比如：  

	aw -> awatch
	bt -> backtrace
	dir -> directory
	disas -> disassemble
	fin -> finish
	ig -> ignore
	ni -> nexti
	rw -> rwatch
	si -> stepi
	tb -> tbreak
	wa -> watch
	win -> winheight
	
另外，如果直接按回車鍵，會重複執行上一次的命令。

## 貢獻者

xmj

nanxiao

