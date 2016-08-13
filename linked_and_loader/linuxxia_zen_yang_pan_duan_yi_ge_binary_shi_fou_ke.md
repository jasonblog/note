# Linux下怎样判断一个binary是否可以debug


用下面的命令即可，如果可以debug，则会显示debug相关的一些信息，否则没有相关信息。

```
objdump --syms your-binary | grep debug
```

或
```
objdump -t your-binary | grep debug
```


示例：



检查一个可以debug的binary：

```sh
[root@ampcommons02 test]# objdump -t t-debug | grep debug
t-debug:     file format elf64-x86-64
0000000000000000 l    d  .debug_aranges 0000000000000000              .debug_aranges
0000000000000000 l    d  .debug_pubnames        0000000000000000              .debug_pubnames
0000000000000000 l    d  .debug_info    0000000000000000              .debug_info
0000000000000000 l    d  .debug_abbrev  0000000000000000              .debug_abbrev
0000000000000000 l    d  .debug_line    0000000000000000              .debug_line
0000000000000000 l    d  .debug_str     0000000000000000              .debug_str
0000000000000000 l    d  .debug_pubtypes        0000000000000000              .debug_pubtypes
0000000000000000 l    d  .debug_ranges  0000000000000000              .debug_ranges
```

检查一个不能debug的binary：


```sh
[root@ampcommons02 test]# objdump -t t-release | grep debug  
```