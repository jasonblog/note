# Linux 共用程式庫的組織


##共用程式庫相容性
共用程式庫的更新分兩類

- 相容更新 - 所有原有的介面都保持不變
- 不相容更新 - 更新修改了原有的介面

這裡的介面是二進位介面 ABI (Application Binary Interface)，要破壞一個共用程式庫的 ABI 十分容易，保持相容卻十分困難，對 C++ 來說要維持相容性更困難。

##共用程式庫版本命名

`libname.so.x.y.z`

前面固定字首 lib，name 是程式庫的名稱，字尾 .so，最後面是版本序號，x 是主版本序號 (Major Version Number)，y 是次版本序號 (Minor Version Number)，z 是發行版本序號 (Release Version Number)。
- 主版本序號
代表程式庫的重大升級，主版本序號不同的程式庫是不相容的，所以有用到的程式必須修改並且重新編譯
- 次版本序號
代表程式庫的增量升級，就是增加新的介面而沒有動到舊的介面，所以高次版本序號可以向後相容低次版本序號的程式庫
- 發行版本序號
代表程式庫一些錯誤修正或者效能改進，而且沒有增加新的介面或修改舊的介面

Note：Glibc 不使用這種規則，而是 libc-x.y.z.so，ld-x.y.z.so

##SO-NAME
Solaris 和 Linux 採用 SO-NAME 的命名機制來記錄共用程式庫的依賴關係，在 Linux 中，系統會為每個共用程式庫在它所在的目錄建立一個叫 SO-NAME 的軟式連結 (Symbol Link)，例如共用程式庫
`/lib/libname.so.x.y.z` 的 SO-NAME 為` libname.so.x`，系統會在 
`/lib` 底下建立 `/lib/libname.so.x` 指向 `/lib/libname.so.x.y.z`，ldconfig 可以根據預設的共用程式庫目錄，更新系統所有的軟式連結。

##連結名稱
使用編譯器連結一個 `libname.so.x.y.z` 的時候，只要在命令列裡指定 `-lname` 即可，編譯器會在系統中的相關路徑 (-L 參數指定)，搜尋最新的 ‘name’ 程式庫，這個 ‘name’ 又稱為共用程式庫的連結名稱 (Link Name)

##符號版本
為瞭解決次版本序號交會問題 (Minor-revision Rendezvous Problem) 而使用的機制，基本概念是讓每個匯出匯入符號都有一個相關聯的版本序號，Linux 下的 Glibc 從 2.1 之後開始支援。

##Solaris 中的符號版本機制
Solaris 的 ld 連結器為共用程式庫增加了版本機制 (Versioning) 和範圍機制 (Scoping)，程式設計師可以在連結共用程式庫時編寫符號版本腳本 (version script) 的檔案

```sh
SUNW_1.1 {
global:
    pop;
    push;
}

SUNWprivate {
global:
    __pop;
    __push;
local:
    *;
}
```


在上面的檔案中，定義了兩個符號集合 SUNW_1.1，SUNWprivate，並且也定義了符號的範圍，假如程式庫升級了，而且多了一個全域函式 swap，那麼上面的檔案可以加上

```sh
SUNW_1.2 {
global:
    swap;
} SUNW_1.1;
```
意思是 1.2 版的共用程式庫增加了 swap 介面，而且繼承了 1.1 的所有介面

##GCC 對 Solaris 符號版本機制的擴充

GCC 在 Solaris 系統中的符號版本機制的基礎上提供了兩個擴充
- 提供 .symver 的組語巨集指令來指定符號的版本

把符號 add 指定為符號標籤 VERS_1.1
```sh
asm(".symver add, add@VERS_1.1");
```
- 允許同一個符號的不同版本存在同一個共用程式庫中 (類似多載)

```sh
asm(".symver old_printf, printf@VERS_1.1");
asm(".symver new_printf, printf@VERS_1.2");
```

連結器可以根據符合程式版本序號的符號來進行連結，使用 1.1 版 printf 的程式會被連結到 old_printf，1.2 版的則連結到 new_printf


##共用程式庫系統路徑
大多數的開源作業系統都遵守 FHS (File Hierarchy Standard) 的標準，這個標準規定了系統中的系統檔案應該如何存放，這有利於各個開源作業系統之間的相容性，而 FHS 規定系統中主要 3 個存放共用程式庫的位置
- /lib
存放系統最關鍵和基礎的共用程式庫，例如動態連結器，C 執行階段程式庫，數學程式庫還有系統啟動時需要的程式庫
- /usr/lib
存放非系統執行時所需要的關鍵性的共用程式庫，還有開發時可能會用到的靜態程式庫或目的檔
- /usr/local/lib
存放跟作業系統本身並不十分相關的程式庫，例如第三方程式的共用程式庫

##共用程式庫搜尋過程

在 Linux 系統中，動態連結器是 `/lib/ld-linux.so.X`，程式所依賴的共用物件全部由動態連結器負責載入和初始化，任何一個動態連結的模組所依賴的模組路徑保存在 .dynamic 區段裡面，由 DT_NEED 類型的項目表示，如果 DT_NEED 是絕對路徑，動態連結器就照這個路徑去搜尋，如果是相對路徑，則會在 /lib，/usr/lib 和 /etc/ld.so.conf 指定的目錄中搜尋

前面提到的 ldconfig，在更新 SO-NAME 的時候會將 SO-NAME 收集起來，集中存放到 /etc/ld.so.cache 檔案裡面，加快共用程式庫的搜尋過程，很多軟體的安裝程式在系統裡面安裝共用程式庫以後都會呼叫 ldconfig。

##環境變數
###LD_LIBRARY_PATH
LD_LIBRARY_PATH 可以改變共用程式庫的搜尋路徑，動態連結器搜尋共用程式庫的順序為
- LD_LIBRARY_PATH 指定的路徑，路徑間由冒號隔開，沒指定則忽略
- 路徑快取檔 /etc/ld.so.cache 指定的路徑
- 預設共用程式庫目錄，先 /usr/lib，然後 /lib

```sh
$ LD_LIBRARY_PATH=/home/user /bin/ls
```

等同於
```sh
$ /lib/ld-linux.so.X -library-path /home/user /bin/ls
```
##LD_PRELOAD
LD_PRELOAD 可以指定預先載入的一些共用程式庫或是目的檔，它的順序比 LD_LIBRARY_PATH 還要優先，無論程式是否需要，LD_PRELOAD 指定的共用程式庫或目的檔都會被載入，而且載入的全域符號會覆蓋後面載入的同名全域符號，對程式的除錯或測試非常有用。
##LD_DEBUG


LD_DEBUG 可以打開動態連結器的除錯功能，可以設定的值如下

```sh
bindings     顯示動態連結的符號連結過程
libs         顯示共用程式庫的搜尋過程
files        顯示載入過程
versions     顯示符號的版本依賴關係
reloc        顯示重定過程
symbols      顯示符號表搜尋過程
statistics   顯示動態連結過程中的各種統計資訊
all          顯示以上所有資訊
help         顯示上面的各種選項的說明資訊
```
用法如下
```sh
$ LD_DEBUG=files /bin/ls
```

##共用程式庫的建立
建立共用程式庫 libfoo.so.1.0.0 的指令如下

```sh
gcc -shared -fPIC -Wl,-soname,libfoo.so.1 -o libfoo.so.1.0.0 libfoo1.c libfoo2.c
```

```sh
-shared 輸出結果是共用程式庫類型的
-fPIC   使用與位址無關的程式碼 (Poition Independent Code)
-Wl     把後面的 -soname,libfoo.so.1 傳給連結器，指定 SO-NAME
```

##清除符號資訊
正常情況下編譯出來的共用程式庫或可執行檔會帶有符號資訊和除錯資訊，`strip libname.so` 可以清除這些資訊
ld 指令的 -s 和 -S 參數也有相同效果，-s 消除所有符號資訊，-S 消除除錯符號資訊，可以在 GCC 中透過 -Wl,-s 和 -Wl,-S 傳給 ld 這兩個參數

