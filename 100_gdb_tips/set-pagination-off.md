# 輸出信息多時不會暫停輸出

## 技巧
有時當gdb輸出信息較多時，gdb會暫停輸出，並會打印“`---Type <return> to continue, or q <return> to quit---`”這樣的提示信息，如下面所示：  

	 81 process 2639102      0xff04af84 in __lwp_park () from /usr/lib/libc.so.1
	 80 process 2573566      0xff04af84 in __lwp_park () from /usr/lib/libc.so.1
	---Type <return> to continue, or q <return> to quit---Quit



解決辦法是使用“`set pagination off`”或者“`set height 0`”命令。這樣gdb就會全部輸出，中間不會暫停。  
參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Screen-Size.html).

## 貢獻者

nanxiao
