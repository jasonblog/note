# 打印內存的值
## 例子
	#include <stdio.h>

	int main(void)
	{
	        int i = 0;
	        char a[100];
	
	        for (i = 0; i < sizeof(a); i++)
	        {
	                a[i] = i;
	        }
	
	        return 0;
	}


## 技巧
gdb中使用“`x`”命令來打印內存的值，格式為“`x/nfu addr`”。含義為以`f`格式打印從`addr`開始的`n`個長度單元為`u`的內存值。參數具體含義如下：  
a）n：輸出單元的個數。  
b）f：是輸出格式。比如`x`是以16進制形式輸出，`o`是以8進制形式輸出,等等。  
c）u：標明一個單元的長度。`b`是一個`byte`，`h`是兩個`byte`（halfword），`w`是四個`byte`（word），`g`是八個`byte`（giant word）。  

以上面程序為例：  
（1） 以16進制格式打印數組前`a`16個byte的值：  

	(gdb) x/16xb a
	0x7fffffffe4a0: 0x00    0x01    0x02    0x03    0x04    0x05    0x06    0x07
	0x7fffffffe4a8: 0x08    0x09    0x0a    0x0b    0x0c    0x0d    0x0e    0x0f
（2） 以無符號10進制格式打印數組`a`前16個byte的值：  

	(gdb) x/16ub a
	0x7fffffffe4a0: 0       1       2       3       4       5       6       7
	0x7fffffffe4a8: 8       9       10      11      12      13      14      15
（3） 以2進制格式打印數組前16個`a`byte的值：  

	(gdb) x/16tb a
	0x7fffffffe4a0: 00000000        00000001        00000010        00000011        00000100        00000101        00000110        00000111
	0x7fffffffe4a8: 00001000        00001001        00001010        00001011        00001100        00001101        00001110        00001111
（4）  以16進制格式打印數組`a`前16個word（4個byte）的值：  

	(gdb) x/16xw a
	0x7fffffffe4a0: 0x03020100      0x07060504      0x0b0a0908      0x0f0e0d0c
	0x7fffffffe4b0: 0x13121110      0x17161514      0x1b1a1918      0x1f1e1d1c
	0x7fffffffe4c0: 0x23222120      0x27262524      0x2b2a2928      0x2f2e2d2c
	0x7fffffffe4d0: 0x33323130      0x37363534      0x3b3a3938      0x3f3e3d3c



參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Memory.html).

## 貢獻者

nanxiao
