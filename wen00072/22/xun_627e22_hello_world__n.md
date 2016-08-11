# 尋找"Hello World\n"

很多人應該知道學C語言第一個程式
- hello.c

```c 
#include <stdio.h>

int main()
{
	printf("Hello world.\n");

	return 0;
}
```

不知道有沒有會問，那個&quot;Hello world\n&quot;放在什麼地方？我承認我用了很多年，到最近才開始去想這個問題。我想再加碼討論不同情況的&quot;Hello world\n&quot;執行檔會放在什麼地方？接下來我們一個一個討論吧

## 共用Makefile

- Makefile

```sh
CFLAGS=-Wall -Werror -g
TARGET=hello
OBJS=$(patsubst %, %.o, $(TARGET))

all: $(TARGET)

%.o: %.c
	$(CC) -o $(patsubst %.o, %, $@) $^

clean:
	rm *.o *~ $(TARGET) -f
```

看不懂語法？唔，我也想不出來為什麼當初這樣寫，應該是剛學`patsubst`所以到處都想用兩下。不過只有幾行，花個時間估狗一下吧？

## 版本一

```c
#include <stdio.h>

int main()
{
	printf("Hello world.\n");

	return 0;
}
```

要知道&quot;Hello world.\n&quot;放在那邊，可以反組譯一下，組合語言下沒有祕密。

```sh
000000000040052d <main>:
  40052d:       55                      push   %rbp
  40052e:       48 89 e5                mov    %rsp,%rbp
  400531:       bf d4 05 40 00          mov    $0x4005d4,%edi
  400536:       e8 d5 fe ff ff          callq  400410 <puts@plt>
  40053b:       b8 00 00 00 00          mov    $0x0,%eax
  400540:       5d                      pop    %rbp
  400541:       c3                      retq   
  400542:       66 2e 0f 1f 84 00 00    nopw   %cs:0x0(%rax,%rax,1)
  400549:       00 00 00 
  40054c:       0f 1f 40 00             nopl   0x0(%rax)

```
我們可以看到沒有`printf`而是出現`puts`，目前只能猜測在沒有format string的情況下gcc會把`printf`換成`puts`，原因可能和效能有關係。

從[這邊](http://wiki.osdev.org/Calling_Conventions#Cheat_Sheets)關於System V X86_64的表格中我們可以看到，參數傳遞使用的暫存器依序為`rdi`, `rsi`, `rdx`, `rcx`, `r8`, `r9`

所以我們可以看到`0x4005d4`會被傳進`puts`，那麼`0x4005d4`在那邊呢？我們可以從symbol table中推測應該在`.rodata` section。

```
$ objdump -t hello
...
00000000004005d0 l    d  .rodata	0000000000000000              .rodata
00000000004005e4 l    d  .eh_frame_hdr	0000000000000000              .eh_frame_hdr
...
```

接下來我們去看`.rodata`裡面的內容，果然找到&quot;Hello world.\n&quot;

```sh
$ objdump -s -j .rodata hello

hello:     file format elf64-x86-64

Contents of section .rodata:
 4005d0 01000200 48656c6c 6f20776f 726c6400  ....Hello world.
```

當然這樣是不太夠，再加碼：

```sh
$ objdump -h hello | grep ro -n2
32- 13 .fini         00000009  00000000004005c4  00000000004005c4  000005c4  2**2
33-                  CONTENTS, ALLOC, LOAD, READONLY, CODE
34: 14 .rodata       00000010  00000000004005d0  00000000004005d0  000005d0  2**2
35-                  CONTENTS, ALLOC, LOAD, READONLY, DATA
36- 15 .eh_frame_hdr 00000034  00000000004005e0  00000000004005e0  000005e0  2**2
```

這邊顯示的資料說`.rodata`是唯讀的。也就是說有人想寫這塊記憶體就會ＧＧ。幸運的是這個statement似乎很難去改字串資料。


## 版本二

```c
#include <stdio.h>

int main()
{
	char *str = "Hello world.\n";
	printf("%s", str);

	return 0;
}
```

一樣需要先來反組譯一下，組合語言下沒有祕密。直接挑重點。

```sh
$ objdump -d hello
...
  400535:       48 c7 45 f8 e4 05 40    movq   $0x4005e4,-0x8(%rbp)
  40053c:       00 
  40053d:       48 8b 45 f8             mov    -0x8(%rbp),%rax
  400541:       48 89 c6                mov    %rax,%rsi
  400544:       bf f2 05 40 00          mov    $0x4005f2,%edi
  400549:       b8 00 00 00 00          mov    $0x0,%eax
  40054e:       e8 bd fe ff ff          callq  400410 <printf@plt>
```

可以看到這次使用了`printf`，而且傳了兩個參數，第一個是`0x4005f2`，第二個是`0x4005e4`。這邊要注意的是，`%rsi`這邊感覺有點脫褲子放屁，不知道為什麼不直接`movq  $0x4005e4, %rsi`。不管怎樣，如第一版方式看看這兩個位址在那個section。

```sh
$ objdump -t hello
...
00000000004005e0 l    d  .rodata	0000000000000000              .rodata
00000000004005f8 l    d  .eh_frame_hdr	0000000000000000              .eh_frame_hdr
...
```

看起來又是`.rodata`，所以我們再看`rodata`裡面放什麼東西。

```sh
$ objdump -s -j .rodata hello

hello:     file format elf64-x86-64

Contents of section .rodata:
 4005e0 01000200 48656c6c 6f20776f 726c640a  ....Hello world.
 4005f0 00257300                             .%s.            
```

所以你可以看到，真正的行為是把`"%s"`和`Hello World\n`這兩個字串的位址傳給`printf`


前面說`.rodata`是read only。那麼我們故意改兩下看看。
改看看放在.rodata的值

```c
#include <stdio.h>

int main()
{
	char *str = "Hello world\n";
	printf("%s", str);
    *str = 'Q';

  return 0;
}
```


結果就會出現組裝工最好的朋友：`Segmentation fault (core dumped)`
```
$ ./hello 
Hello world
Segmentation fault (core dumped)
```


## 版本三

```c
#include <stdio.h>

int main()
{
	char str[] = "Hello world.\n";
	printf("%s", str);

	return 0;
}
```

這個可以看到是一個有初始值的陣列，而在函數內的變數會放在stack。所以我這樣測，下面結果又臭又長，用力找可以看到（精確的來說，湊到）&quot;Hello world.&quot;的字串。

```sh
$ objdump -s -j .text hello

hello:     file format elf64-x86-64

Contents of section .text:
 4004b0 31ed4989 d15e4889 e24883e4 f0505449  1.I..^H..H...PTI
 4004c0 c7c07006 400048c7 c1000640 0048c7c7  ..p.@.H....@.H..
 4004d0 9d054000 e8b7ffff fff4660f 1f440000  ..@.......f..D..
 4004e0 b84f1060 0055482d 48106000 4883f80e  .O.`.UH-H.`.H...
 4004f0 4889e577 025dc3b8 00000000 4885c074  H..w.]......H..t
 400500 f45dbf48 106000ff e00f1f80 00000000  .].H.`..........
 400510 b8481060 0055482d 48106000 48c1f803  .H.`.UH-H.`.H...
 400520 4889e548 89c248c1 ea3f4801 d048d1f8  H..H..H..?H..H..
 400530 75025dc3 ba000000 004885d2 74f45d48  u.]......H..t.]H
 400540 89c6bf48 106000ff e20f1f80 00000000  ...H.`..........
 400550 803df10a 20000075 11554889 e5e87eff  .=.. ..u.UH...~.
 400560 ffff5dc6 05de0a20 0001f3c3 0f1f4000  ..].... ......@.
 400570 48833da8 08200000 741eb800 00000048  H.=.. ..t......H
 400580 85c07414 55bf200e 60004889 e5ffd05d  ..t.U. .`.H....]
 400590 e97bffff ff0f1f00 e973ffff ff554889  .{.......s...UH.
 4005a0 e54883ec 2064488b 04252800 00004889  .H.. dH..%(...H.
 4005b0 45f831c0 48b84865 6c6c6f20 776f4889  E.1.H.Hello woH.
 4005c0 45e0c745 e8726c64 2e66c745 ec0a0048  E..E.rld.f.E...H
 4005d0 8d45e048 89c6bf84 064000b8 00000000  .E.H.....@......
 4005e0 e89bfeff ffb80000 0000488b 55f86448  ..........H.U.dH
 4005f0 33142528 00000074 05e872fe ffffc9c3  3.%(...t..r.....
 400600 41574189 ff415649 89f64155 4989d541  AWA..AVI..AUI..A
 400610 544c8d25 f8072000 55488d2d f8072000  TL.%.. .UH.-.. .
 400620 534c29e5 31db48c1 fd034883 ec08e80d  SL).1.H...H.....
 400630 feffff48 85ed741e 0f1f8400 00000000  ...H..t.........
 400640 4c89ea4c 89f64489 ff41ff14 dc4883c3  L..L..D..A...H..
 400650 014839eb 75ea4883 c4085b5d 415c415d  .H9.u.H...[]A\A]
 400660 415e415f c366662e 0f1f8400 00000000  A^A_.ff.........
 400670 f3c3                                 .. 
```

當然這樣證據不夠，還是反組譯一下好了。一樣挑重點。

```sh
$ objdump -d hello
...
  4005b4:	48 b8 48 65 6c 6c 6f 	movabs $0x6f77206f6c6c6548,%rax
  4005bb:	20 77 6f 
  4005be:	48 89 45 e0          	mov    %rax,-0x20(%rbp)
  4005c2:	c7 45 e8 72 6c 64 2e 	movl   $0x2e646c72,-0x18(%rbp)
  4005c9:	66 c7 45 ec 0a 00    	movw   $0xa,-0x14(%rbp)
  4005cf:	48 8d 45 e0          	lea    -0x20(%rbp),%rax
  4005d3:	48 89 c6             	mov    %rax,%rsi
  4005d6:	bf 84 06 40 00       	mov    $0x400684,%edi
  4005db:	b8 00 00 00 00       	mov    $0x0,%eax
  4005e0:	e8 9b fe ff ff       	callq  400480 <printf@plt>
...
```

先來猜測這一段在做啥：

* 呼叫printf
* 所以要帶參數，也就是&quot;%s&quot;和`str`

`str`的部份可以看到，其實存在stack中
首先是`movabs $0x6f77206f6c6c6548,%rax`

請對照[ASCII 表](http://en.wikipedia.org/wiki/ASCII#ASCII_printable_code_chart)
因為x86用[little endian](http://en.wikipedia.org/wiki/Endianness)，所以請從右到左來看operand `0x6f77206f6c6c6548`

* `0x48`：`H`
* `0x65`：`e`
* `0x6c`：`l`
* `0x6c`：`l`
* `0x6f`：`o`
* `0x20`：` `
* `0x77`：`w`
* `0x77`：`o`

接下來是`4005c2:	c7 45 e8 72 6c 64 2e 	movl   $0x2e646c72,-0x18(%rbp)`
一樣，對照表格可以看到
* `0x72`：`r`
* `0x6c`：`l`
* `0x64`：`d`
* `0x2e`：`.`

最後是`movw   $0xa,-0x14(%rbp)`
* `0xa`：`\n`

把資料存到stack後，再把stack address傳給printf，相對動作是
```
  4005cf:       48 8d 45 e0             lea    -0x20(%rbp),%rax
  4005d3:       48 89 c6                mov    %rax,%rsi
```

而`"%s"`存在`.rodata`，從`objdump -t hello`可以看到`.rodata`位址是`0x400680`，內容是

```
$ objdump -s -j .rodata hello

hello:     file format elf64-x86-64

Contents of section .rodata:
 400680 01000200 257300                      ....%s. 
```
也就是說`"%s"`放在`0x400684`，因此直接把該位址當作參數傳給printf

```
  4005d6:       bf 84 06 40 00          mov    $0x400684,%edi
```

最後做個總結，這個版本的&quot;Hello world.\n&quot;程式本身&quot;hardcode&quot;到stack內。更簡單的說，&quot;Hello world.\n&quot;放在`.text`裡面。


## 版本四

```c
#include <stdio.h>

int main()
{
	static char str[] = "Hello world.\n";
	printf("%s", str);

	return 0;
}
```

一樣反組譯一下。直接挑重點。

```sh
$ objdump -d hello
...
  400531:       be 40 10 60 00          mov    $0x601040,%esi
  400536:       bf d4 05 40 00          mov    $0x4005d4,%edi
  40053b:       b8 00 00 00 00          mov    $0x0,%eax
  400540:       e8 cb fe ff ff          callq  400410 <printf@plt>
...
```

`0x601040`和`0x4005d4`在那邊呢？我們可以看一下section資訊，可以看到這兩個分別落在`.data`和`.rodata`

```
$ objdump -t hello
...
00000000004005d0 l    d  .rodata	0000000000000000              .rodata
0000000000601030 l    d  .data	0000000000000000              .data
```

馬上來看`.data`和`.rodata`的內容

```sh
$ objdump -s -j .rodata hello

hello:     file format elf64-x86-64

Contents of section .rodata:
 4005d0 01000200 257300                      ....%s.         

$ objdump -s -j .data hello

hello:     file format elf64-x86-64

Contents of section .data:
 601030 00000000 00000000 00000000 00000000  ................
 601040 48656c6c 6f20776f 726c642e 0a00      Hello world...  
```

## 結論
依照程式的寫法，你的資料會放在不同的section。沒弄好就會把程式搞爛，有興趣的朋友可以自己設計其他實驗看看。

## 補充 2015-01-29
感謝Scott大大的補充。從上面的反組譯中可以看到在呼叫printf之前都會去把%eax設成零。Scott大大提醒以後才知道這樣設定是有原因的。說明如下

首先printf是一個有趣的函數，有沒有人想過為什麼他的參數的數量可以變動？我先承認我這兩年才去瞭解，基本上這東西叫作`va_arg`，細節就不談了，有興趣`man va_arg`就好了，那天想起來或是有人敲碗再解釋。

回到前面，因為printf是非固定參數數量，而ABI中有又有規範`va_arg`時候該如何傳遞。在[X86的ABI](http://www.x86-64.org/documentation/abi.pdf)規範3.5.7中提到，在傳遞這樣參數的時候，需要把要傳遞的浮點型態變數數量放在eax暫存器中。而這次範例中的printf恰巧都沒有浮點型態變數，所以將eax設成0。Scott大大沒說我還以為單純是歸零的動作orz。

## 參考資料

* [stackoverflow: How can I examine contents of a data section of an ELF file on Linux?](http://stackoverflow.com/questions/1685483/how-can-i-examine-contents-of-a-data-section-of-an-elf-file-on-linux)
* [Endianness](http://en.wikipedia.org/wiki/Endianness)
* [ASCII 表](http://en.wikipedia.org/wiki/ASCII#ASCII_printable_code_chart)
* [OSDev: Calling Conventions](http://wiki.osdev.org/Calling_Conventions#Cheat_Sheets)
