# GDB實用教學：自動化你的debug


如果不曉得什麼是GDB，建議從「`Linux 除錯利器 - GDB 簡介」`開始學習，此篇是給知道什麼是GDB，但總覺得GDB不是那麼好用的開發者，重新認識GDB，跟隨著影片的腳步，重現一次。



GDB也有GUI
雖然GDB是在終端機中使用，輸入l可以列出目前的程式碼，但大家一定還是覺得很難用，所以就有了tui的出現：

GDB也有GUI
雖然GDB是在終端機中使用，輸入l可以列出目前的程式碼，但大家一定還是覺得很難用，所以就有了tui的出現：

```sh
$ gdb -tui
or
$ gdbtui
```

都可以用指令的方式進入GDB的圖形介面。
但如果只以單純的
```
$ gdb
```

進入的話，只要按下ctrl + x +a 就可以了。


這邊以一個簡單的helloworld程式(hello.c)來說明：

```c
#include <stdio.h>

int main(void)
{
    int i = 0;
    printf("Hello, world\n");
    printf("i is %d\n", i);
    i++;
    printf("i is now %d\n", i);
}
```


接下來在終端機輸入： 
```
$ gcc -g -O0 -o hello hello.c
$ gdb ./hello
```

```
(gdb) start 
```
然後按下ctrl + x +a 後，就變成這樣了！


這時候，如果有遇到破圖，按下ctrl + L 就可以刷新畫面。
如要關閉tui，在按一次ctrl + x +a 即可
到這裡，大部份已知有tui可以用的人，都覺得沒什麼！
不過驚喜的就是：如果按ctrl + x + 2會出現組合語言(類似objdump出來的結果)的視窗！
再按一次相同的組合鍵，會跳出register的內容！
而且是以循環的方式呈現。

於gdbtui中，沒辦法使用 方向鍵上、方向鍵下 來使用之前使用過的指令，記得改用ctrl + p 和 ctrl +n來使用歷史指令。
或是透過ctrl + x + o 來切換active window，方向鍵上下，可能就會有你想要的功能了！


GDB也有python可以用
先來看個小示範，一樣是上面的hello.c

```sh
(gdb) r
(gdb) b hello.c:8
(gdb) c
(gdb) python
> print('hello python')
```

此時按下ctrl+D(或輸入end) 退出python，就可以看到輸出！

