# GDB下查看記憶體指令(x指令)


可以使用examine命令(簡寫是x)來查看內存地址中的值。x命令的語法如下所示：

 
```c
x/<n/f/u> <addr>
```

n、f、u是可選的參數。

 

 

n是一個正整數，表示需要顯示的內存單元的個數，也就是說從當前地址向後顯示幾個內存單元的內容，一個內存單元的大小由後面的u定義。

 

 

f 表示顯示的格式，參見下面。如果地址所指的是字符串，那麼格式可以是s，如果地十是指令地址，那麼格式可以是i。

 

 

u 表示從當前地址往後請求的字節數，如果不指定的話，GDB默認是4個bytes。u參數可以用下面的字符來代替，b表示單字節，h表示雙字節，w表示四字 節，g表示八字節。當我們指定了字節長度後，GDB會從指內存定的內存地址開始，讀寫指定字節，並把其當作一個值取出來。

 

 

<addr>表示一個內存地址。

 

 

注意：嚴格區分n和u的關係，n表示單元個數，u表示每個單元的大小。

 

n/f/u三個參數可以一起使用。例如：

命令：x/3uh 0x54320 表示，從內存地址0x54320讀取內容，h表示以雙字節為一個單位，3表示輸出三個單位，u表示按十六進制顯示。

 

輸出格式

一般來說，GDB會根據變量的類型輸出變量的值。但你也可以自定義GDB的輸出的格式。例如，你想輸出一個整數的十六進制，或是二進制來查看這個整型變量的中的位的情況。要做到這樣，你可以使用GDB的數據顯示格式：

``` sh
x 按十六進制格式顯示變量。

d 按十進制格式顯示變量。

u 按十六進制格式顯示無符號整型。

o 按八進制格式顯示變量。

t 按二進制格式顯示變量。

a 按十六進制格式顯示變量。

c 按字符格式顯示變量。

f 按浮點數格式顯示變量。
```
 
```sh
(gdb) help x

Examine memory: x/FMT ADDRESS.

ADDRESS is an expression for the memory address to examine.

FMT is a repeat count followed by a format letter and a size letter.

Format letters are o(octal), x(hex), d(decimal), u(unsigned decimal),

t(binary), f(float), a(address), i(instruction), c(char) and s(string).

Size letters are b(byte), h(halfword), w(word), g(giant, 8 bytes).

The specified number of objects of the specified size are printed

according to the format.

 

Defaults for format and size letters are those previously used.

Default count is 1. Default address is following last thing printed

with this command or "print".

(gdb) p f1

$4 = 8.25

(gdb) p f2

$5 = 125.5

(gdb) x/x &f1

0xbffff380:    0x41040000

(gdb) x/xw &f1

0xbffff380:    0x41040000

(gdb) x/xw &f2

0xbffff384:    0x42fb0000

(gdb) x/2xw &f1

0xbffff380:    0x41040000    0x42fb0000

(gdb) x/4xw &f1

0xbffff380:    0x41040000    0x42fb0000    0xbffff408    0x00bcba66

(gdb) x/tw &f1

0xbffff380:    01000001000001000000000000000000

(gdb) x/2tw &f1

0xbffff380:    01000001000001000000000000000000    01000010111110110000000000000000
```