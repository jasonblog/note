# 如何在 Linux 上使用自行編譯的第三方函式庫


##以使用 LevelDB 為例。

抓好並編好相關檔案，編譯方式見第三方函式庫附的說明：

```sh
$ ls include/  # header files
leveldb/
$ ls out-shared/libleveldb.so*  # shared library
out-shared/libleveldb.so@  out-shared/libleveldb.so.1@  out-shared/libleveldb.so.1.20*
```

下面的例子用 clang++ 編譯，這裡用到的參數和 g++ 一樣。

##問題一：找不到 header

```sh
$ clang++ sample.cpp
sample.cpp:5:10: fatal error: 'leveldb/db.h' file not found
#include "leveldb/db.h"
         ^
1 error generated.
```

解法：用 -I 指定 header 位置

##問題二：找不到 shared library

```sh
$ clang++ sample.cpp -I include/
/tmp/sample-2e7dd8.o: In function `main':
sample.cpp:(.text+0x1e): undefined reference to `leveldb::Options::Options()'
sample.cpp:(.text+0x6f): undefined reference to `leveldb::DB::Open(leveldb::Options const&, std::string const&, leveldb::DB**)'
sample.cpp:(.text+0x10c): undefined reference to `leveldb::Status::ToString() const'
sample.cpp:(.text+0x7d0): undefined reference to `leveldb::Status::ToString() const'
clang: error: linker command failed with exit code 1 (use -v to see invocation)
```

要求 linker 連結 libleveldb.so (linker 的參數由 clang++ / g++ 轉傳)：

```sh
$ clang++ sample.cpp -I include/ -l leveldb
/usr/bin/ld: cannot find -lleveldb
clang: error: linker command failed with exit code 1 (use -v to see invocation)
```

但 compiler 說找不到要連結的 library

補上 libleveldb 的位置：

```sh
$ clang++ sample.cpp -I include/ -l leveldb -L out-shared/
```

##問題三：執行時找不到 shared library

```sh
$ ./a.out
./a.out: error while loading shared libraries: libleveldb.so.1: cannot open shared object file: No such file or directory
```

編出 executable file 或 shared library 表示 static linker 成功，但執行時會用到 dynamic linker 載入函式庫。這錯誤訊息是dynamic linker 回報的。

用 ldd 可以檢查 shared library 的路徑是否正確：

```sh
$ ldd a.out | grep leveldb
        libleveldb.so.1 => not found
```

幾種解法：

###1. 用 LD_LIBRARY_PATH 指定位置 (man ld.so 查看細節)

```sh
$ LD_LIBRARY_PATH=`pwd`/out-shared ./a.out
```

若 out-shared 的位置有固定的話，可以在 /.bashrc 加上

```sh
export LD_LIBRARY_PATH=/path/to/out-shared 
```

###2. 將 library path 寫到 executable 裡 (man ld.sh 查看細節)：

```sh
$ clang++ sample.cpp -I include/ -l leveldb -L out-shared/ -Wl,-rpath,`pwd`/out-shared 
$ objdump -p a.out | grep PATH  # 確認有記錄
  RPATH                /home/fcamel/dev/study/leveldb/out-shared
$ ldd a.out  | grep leveldb  # 也可用 ldd 確認
        libleveldb.so.1 => /home/fcamel/dev/study/leveldb/out-shared/libleveldb.so.1 (0x00007fc1f091e000)
```

`這裡我用絕對路徑減少潛在的問題`。

###3. 搬到系統函式庫

```sh
$ ldd a.out  | grep leveldb
        libleveldb.so.1 => not found
$ sudo su
$ cp out-shared/libleveldb.so* /usr/lib
$ ldd a.out  | grep leveldb
        libleveldb.so.1 => /usr/lib/libleveldb.so.1 (0x00007f1717026000)
```

但這樣和系統內建的混在一起，不好維護。改放到 /usr/local/lib/leveldb/ 下：

```sh
$ mkdir /usr/local/lib/leveldb
$ cp --preserve=links out-shared/libleveldb.so* /usr/local/lib/leveldb/
$ echo "/usr/local/lib/leveldb" > /etc/ld.so.conf.d/leveldb.conf
$ ldconfig # Update ldconfig's cache
$ ldd a.out | grep leveldb
        libleveldb.so.1 => /usr/local/lib/leveldb/libleveldb.so.1 (0x00007f0314b32000)
```

由 man ldconfig 得知 ldconfig 會讀 /etc/ld.so.conf。我在 Ubuntu 14.04 看到的設定如下：

```sh
$ cat /etc/ld.so.conf
include /etc/ld.so.conf.d/*.conf
```

所以在 /etc/ld.so.conf.d/ 建新檔案寫入 /usr/local/lib/leveldb，然後更新 ldconfig cache 即可。

##參考資料

- The Linux Programming Interface ch41: Fundamentals of Shared Libraries
- man ld.so
- man ldconfig