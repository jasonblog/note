# 使用另一个版本的glibc


glibc是Linux系统的核心库，稍有不慎就会导致系统崩溃。如果在程序中必须使用另一版本的glibc，则需要小心从事。具体来言，是在编译时指定`--rpath`和`--dynamic-linker`，而在运行时指定LIB_LIBRARY_PATH。

```sh
gcc main.c -o main -Wl,--rpath=/path/to/new/glibc/lib \
		-Wl,--dynamic-linker=/path/to/new/glibc/ld-linux.so.2
```

其中ld-linux.so.2在64位系统下可能为ld-linux-x86-64.so.2，它的路径会硬编码到程序中(因此显式指定时换成别的名字也没什么问题)。正是这个原因，导致很多已有程序不能使用新glibc的库。

另外，查看链接问题的两个常用工具是:

- ldd: 查看可执行文件或动态库依赖的其它的库
- strings: 查看库中的字符串，比如`strings /lib64/libc.so.2 | grep GLIBC` 可以查看GLIBC支持的版本。