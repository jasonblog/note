# 使用另一個版本的glibc


glibc是Linux系統的核心庫，稍有不慎就會導致系統崩潰。如果在程序中必須使用另一版本的glibc，則需要小心從事。具體來言，是在編譯時指定`--rpath`和`--dynamic-linker`，而在運行時指定LIB_LIBRARY_PATH。

```sh
gcc main.c -o main -Wl,--rpath=/path/to/new/glibc/lib \
		-Wl,--dynamic-linker=/path/to/new/glibc/ld-linux.so.2
```

其中ld-linux.so.2在64位系統下可能為ld-linux-x86-64.so.2，它的路徑會硬編碼到程序中(因此顯式指定時換成別的名字也沒什麼問題)。正是這個原因，導致很多已有程序不能使用新glibc的庫。

另外，查看鏈接問題的兩個常用工具是:

- ldd: 查看可執行文件或動態庫依賴的其它的庫
- strings: 查看庫中的字符串，比如`strings /lib64/libc.so.2 | grep GLIBC` 可以查看GLIBC支持的版本。