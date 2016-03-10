# Jason note

七、設置顯示選項
GDB中關於顯示的選項比較多，這裡我只例舉大多數常用的選項。
```sh
set print address
set print address on
```

打開地址輸出，當程序顯示函數信息時，GDB會顯出函數的參數地址。系統默認為打開的，如：
```sh
(gdb) f
#0  set_quotes (lq=0x34c78 ">")
at input.c:530
530         if (lquote != def_lquote)
```
```
set print address off
```

關閉函數的參數地址顯示，如：
```sh
(gdb) set print addr off
(gdb) f
#0  set_quotes (lq=">") at input.c:530
530         if (lquote != def_lquote)
```

```sh
show print address
```

查看當前地址顯示選項是否打開。
```sh
set print array
set print array on
```

打開數組顯示，打開後當數組顯示時，每個元素佔一行，如果不打開的話，每個元素則以逗號分隔。這個選項默認是關閉的。與之相關的兩個命令如下，我就不再多說了。
```sh
set print array off
show print array
set print elements 
```

這個選項主要是設置數組的，如果你的數組太大了，那麼就可以指定一個來指定數據顯示的最大長度，當到達這個長度時，GDB就不再往下顯示了。如果設置為0，則表示不限制。
```sh
show print elements
```

查看print elements的選項信息。
```sh
set print null-stop 
```

如果打開了這個選項，那麼當顯示字符串時，遇到結束符則停止顯示。這個選項默認為off。
```sh
set print pretty on
```

如果打開printf pretty這個選項，那麼當GDB顯示結構體時會比較漂亮。如：
```sh
$1 = {
next = 0x0,
flags = {
sweet = 1,
sour = 1
},
meat = 0x54 "Pork"
}
```

```sh
set print pretty off
```

關閉printf pretty這個選項，GDB顯示結構體時會如下顯示：
```sh
$1 = {next = 0x0, flags = {sweet = 1, sour = 1}, meat = 0x54
"Pork"}
```

```sh
show print pretty
```

查看GDB是如何顯示結構體的。
```sh
set print sevenbit-strings 
```

設置字符顯示，是否按「/nnn」的格式顯示，如果打開，則字符串或字符數據按/nnn顯示，如「/065」。
```sh
show print sevenbit-strings
```

查看字符顯示開關是否打開。
```sh
set print union 
```

設置顯示結構體時，是否顯式其內的聯合體數據。例如有以下數據結構：
```c
typedef enum {Tree, Bug} Species;
typedef enum {Big_tree, Acorn, Seedling} Tree_forms;
typedef enum {Caterpillar, Cocoon, Butterfly}
Bug_forms;
struct thing {
    Species it;
    union {
        Tree_forms tree;
        Bug_forms bug;
    } form;
};
struct thing foo = {Tree, {Acorn}};
```

當打開這個開關時，執行 p foo 命令後，會如下顯示：

```sh
$1 = {it = Tree, form = {tree = Acorn, bug = Cocoon}}
```

當關閉這個開關時，執行 p foo 命令後，會如下顯示：
```sh
$1 = {it = Tree, form = {...}}
```
```sh
show print union
```

查看聯合體數據的顯示方式
```sh
set print object 
```

在C++中，如果一個對象指針指向其派生類，如果打開這個選項，GDB會自動按照虛方法調用的規則顯示輸出，如果關閉這個選項的話，GDB就不管虛函數表了。這個選項默認是off。
```sh
show print object
```

查看對象選項的設置。
```sh
set print static-members 
```

這個選項表示，當顯示一個C++對象中的內容是，是否顯示其中的靜態數據成員。默認是on。
```sh
show print static-members
```

查看靜態數據成員選項設置。
```sh
set print vtbl 
```

當此選項打開時，GDB將用比較規整的格式來顯示虛函數表時。其默認是關閉的。
```sh
show print vtbl
```

查看虛函數顯示格式的選項。


