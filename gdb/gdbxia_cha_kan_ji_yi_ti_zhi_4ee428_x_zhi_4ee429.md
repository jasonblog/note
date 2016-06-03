# GDB下查看記憶體指令(x指令)


可以使用examine命令(简写是x)来查看内存地址中的值。x命令的语法如下所示：

 
```c
x/<n/f/u> <addr>
```

n、f、u是可选的参数。

 

 

n是一个正整数，表示需要显示的内存单元的个数，也就是说从当前地址向后显示几个内存单元的内容，一个内存单元的大小由后面的u定义。

 

 

f 表示显示的格式，参见下面。如果地址所指的是字符串，那么格式可以是s，如果地十是指令地址，那么格式可以是i。

 

 

u 表示从当前地址往后请求的字节数，如果不指定的话，GDB默认是4个bytes。u参数可以用下面的字符来代替，b表示单字节，h表示双字节，w表示四字 节，g表示八字节。当我们指定了字节长度后，GDB会从指内存定的内存地址开始，读写指定字节，并把其当作一个值取出来。

 

 

<addr>表示一个内存地址。

 

 

注意：严格区分n和u的关系，n表示单元个数，u表示每个单元的大小。

 

n/f/u三个参数可以一起使用。例如：

命令：x/3uh 0x54320 表示，从内存地址0x54320读取内容，h表示以双字节为一个单位，3表示输出三个单位，u表示按十六进制显示。

 

输出格式

一般来说，GDB会根据变量的类型输出变量的值。但你也可以自定义GDB的输出的格式。例如，你想输出一个整数的十六进制，或是二进制来查看这个整型变量的中的位的情况。要做到这样，你可以使用GDB的数据显示格式：

``` sh
x 按十六进制格式显示变量。

d 按十进制格式显示变量。

u 按十六进制格式显示无符号整型。

o 按八进制格式显示变量。

t 按二进制格式显示变量。

a 按十六进制格式显示变量。

c 按字符格式显示变量。

f 按浮点数格式显示变量。
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