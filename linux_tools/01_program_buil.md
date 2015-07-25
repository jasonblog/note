# 程序構建
目錄

程序構建
配置
編譯
makefile編寫的要點
makefile中的全局自變量
更多選擇 CMake
編譯依賴的庫
g++編譯
安裝
總結
一般源代碼提供的程序安裝需要通過配置、編譯、安裝三個步驟；

配置做的工作主要是檢查當前環境是否滿足要安裝軟件的依賴關係，以及設置程序安裝所需要的初始化信息，比如安裝路徑，需要安裝哪些組件；配置完成，會生成makefile文件供第二步make使用；
編譯是對源文件進行編譯鏈接生成可執行程序；
安裝做的工作就簡單多了，就是將生成的可執行文件拷貝到配置時設置的初始路徑下；
1.1. 配置
查詢可用的配置選項:

```
./configure --help
```

配置路徑:

```
./configure --prefix=/usr/local/snmp
```

–prefix是配置使用的最常用選項，設置程序安裝的路徑；

1.2. 編譯
編譯使用make編譯:

```
make -f myMakefile
```

通過-f選項顯示指定需要編譯的makefile；如果待使用makefile文件在當前路徑，且文件名為以下幾個，則不用顯示指定：

makefile Makefile

makefile編寫的要點
必須滿足第一條規則，滿足後停止
除第一條規則，其他無順序
makefile中的全局自變量
$@目標文件名
@^所有前提名，除副本
@＋所有前提名，含副本
@＜一個前提名
@？所有新於目標文件的前提名
@*目標文件的基名稱
註解

系統學習makefile的書寫規則，請參考 跟我一起學makefile [1]
更多選擇 CMake
CMake是一個跨平臺的安裝（編譯）工具，可以用簡單的語句來描述所有平臺的安裝(編譯過程)。他能夠輸出各種各樣的makefile或者project文件。使用CMake，能夠使程序員從複雜的編譯連接過程中解脫出來。它使用一個名為 CMakeLists.txt 的文件來描述構建過程,可以生成標準的構建文件,如 Unix/Linux 的 Makefile 或Windows Visual C++ 的 projects/workspaces 。

編譯依賴的庫
makefile編譯過程中所依賴的非標準庫和頭文件路徑需要顯示指明:

```
CPPFLAGS -I標記非標準頭文件存放路徑
LDFLAGS  -L標記非標準庫存放路徑
```

如果CPPFLAGS和LDFLAGS已在用戶環境變量中設置並且導出（使用export關鍵字），就不用再顯示指定；

```
make -f myMakefile LDFLAGS='-L/var/xxx/lib -L/opt/mysql/lib'
    CPPFLAGS='-I/usr/local/libcom/include -I/usr/local/libpng/include'
```

警告

鏈接多庫時，多個庫之間如果有依賴，需要注意書寫的順序，右邊是左邊的前提；
g++編譯
```
g++ -o unixApp unixApp.o a.o b.o
```

選項說明：

-o:指明生成的目標文件
-g：添加調試信息
-E: 查看中間文件
應用：查詢宏展開的中間文件：

在g++的編譯選項中，添加 -E選項，然後去掉-o選項 ，重定向到一個文件中即可:

```
g++ -g -E unixApp.cpp  -I/opt/app/source > midfile
```

查詢應用程序需要鏈接的庫:

```
$ldd myprogrammer
    libstdc++.so.6 => /usr/lib64/libstdc++.so.6 (0x00000039a7e00000)
    libm.so.6 => /lib64/libm.so.6 (0x0000003996400000)
    libgcc_s.so.1 => /lib64/libgcc_s.so.1 (0x00000039a5600000)
    libc.so.6 => /lib64/libc.so.6 (0x0000003995800000)
    /lib64/ld-linux-x86-64.so.2 (0x0000003995400000)
```

註解

關於ldd的使用細節，參見 ldd 查看程序依賴庫
1.3. 安裝
安裝做的工作就簡單多了，就是將生成的可執行文件拷貝到配置時設置的初始路徑下:

```
$make install
```

其實 install 就是makefile中的一個規則，打開makefile文件後可以查看程序安裝的所做的工作；

1.4. 總結
configure make install g++

[1]	陳皓 跟我一起寫Makefile http://scc.qibebt.cas.cn/docs/linux/base/%B8%FA%CE%D2%D2%BB%C6%F0%D0%B4Makefile-%B3%C2%F0%A9.pdf
