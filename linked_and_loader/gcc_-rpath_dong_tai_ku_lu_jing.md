# gcc -rpath 動態庫路徑


在C/C++程序裡經常會調用到外部庫函數，最常用的方法莫過於export LD_LIBRARY_PATH，不過使用它存在一些弊端，可能會影響到其它程序的運行。在經歷的大項目中就遇到過，兩個模塊同時使用一外部動態庫，而且版本還有差異，導致其中一模塊出錯，兩模塊是不同時期不同人員分別開發，修正起來費時費力。

對於上述問題，一個比較好的方法是在程序編譯的時候加上參數-Wl,-rpath，`指定編譯好的程序在運行時動態庫的目錄`。這種方法會將動態庫路徑寫入到elf文件中去。

```sh
-Wa,<options>            Pass comma-separated <options> on to the assembler
-Wp,<options>            Pass comma-separated <options> on to the preprocessor
-Wl,<options>             Pass comma-separated <options> on to the linker 
```
可以用ldd命令查看可執行程序鏈接的動態庫路徑。