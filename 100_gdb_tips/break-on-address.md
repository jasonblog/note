# 在程序地址上打斷點 

## 例子

	0000000000400522 <main>:
	  400522:       55                      push   %rbp
	  400523:       48 89 e5                mov    %rsp,%rbp
	  400526:       8b 05 00 1b 00 00       mov    0x1b00(%rip),%eax        # 40202c <he+0xc>
	  40052c:       85 c0                   test   %eax,%eax
	  40052e:       75 07                   jne    400537 <main+0x15>
	  400530:       b8 7c 06 40 00          mov    $0x40067c,%eax
	  400535:       eb 05                   jmp    40053c <main+0x1a>

## 技巧

當調試彙編程序，或者沒有調試信息的程序時，經常需要在程序地址上打斷點，方法為`b *address`。例如：

	(gdb) b *0x400522

詳情參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Specify-Location.html#Specify-Location)

## 貢獻者

xmj

