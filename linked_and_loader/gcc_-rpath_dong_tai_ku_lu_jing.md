# gcc -rpath 动态库路径


在C/C++程序里经常会调用到外部库函数，最常用的方法莫过于export LD_LIBRARY_PATH，不过使用它存在一些弊端，可能会影响到其它程序的运行。在经历的大项目中就遇到过，两个模块同时使用一外部动态库，而且版本还有差异，导致其中一模块出错，两模块是不同时期不同人员分别开发，修正起来费时费力。

对于上述问题，一个比较好的方法是在程序编译的时候加上参数-Wl,-rpath，`指定编译好的程序在运行时动态库的目录`。这种方法会将动态库路径写入到elf文件中去。

```sh
-Wa,<options>            Pass comma-separated <options> on to the assembler
-Wp,<options>            Pass comma-separated <options> on to the preprocessor
-Wl,<options>             Pass comma-separated <options> on to the linker 
```
可以用ldd命令查看可执行程序链接的动态库路径。