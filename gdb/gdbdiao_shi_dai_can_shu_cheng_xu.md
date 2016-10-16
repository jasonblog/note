# gdb調試帶參數程序


一般來說GDB主要調試的是C/C++的程序。要調試C/C++的程序，首先在編譯時，我們必須要 
把調試信息加到可執行文件中。使用編譯 器（cc/gcc/g++）的 -g 參數可以做到這一點。如： 

```sh
> cc -g hello.c -o hello 
> g++ -g hello.cpp -o hello 
```


如果沒有-g，你將看不見程序的函數名、變量名，所代替的全是 運行時的內存地址。當你用 
-g把調試信息加入之後，併成功編譯目標代碼以後，讓我們來看看如何用gdb來調試他。 

啟動 GDB的方法有以下幾種： 

```sh
1、gdb <program> 
program也就是你的執行文件，一般在當然目錄下。 

2、gdb <program> core 
用gdb同時調試一個運行程序和core文件，core是程序非法執行後core dump後產生 
的文件。 

3、gdb <program> <PID> 
如果你的程序是一個服務程序，那麼你可以指定這個服務程序運行時的進程ID。gd 
b會自動attach上去，並調試他。program應該在 PATH環境變量中搜索得到。 
```


GDB啟動時，可以加上一些GDB的啟動開關，詳細的開關可以用gdb -help查看。我在下面只 
例舉一些比較常用的參數： 

```
-symbols <file> 
-s <file> 
從指定文件中讀取符號表。 
```

```
-se file 
從指定文件中讀取符號表信息，並把他用在可執行文件中。 
```

```
-core <file> 
-c <file> 
調試時core dump的core文件。 
```

```
-directory <directory> 
-d <directory> 
加入一個源文件的搜索路徑。默認搜索路徑是環境變量中PATH所定義的路徑。
```


當以gdb <program>方式啟動gdb後，gdb會在PATH路徑和當前目錄中搜索<program>的源文件 
。 如要確認gdb是否讀到源文件，可使用l或list命令，看看gdb是否能列出源代碼。 

在gdb中，運行程序使用r或是run命令。 程序的運行，你有可能需要設置下面四方面的事。 
```sh
1、程序運行參數。 
set args 可指定運行時參數。（如：set args 10 20 30 40 50） 
show args 命令可以查看設置好的運行參數。 

2、 運行環境。 
path <dir> 可設定程序的運行路徑。 
show paths 查看程序的運行路徑。 
set environment varname [=value] 設置環境變量。如：set env USER=hchen 
show environment [varname] 查看環境變量。 

3、工作目錄。 
cd <dir> 相當於shell的cd命令。 
pwd 顯示當前的所在目錄。 

4、程序的輸入輸出。 
info terminal 顯示你程序用到的終端的模式。 
使用重定向控制程序輸出。如：run > outfile 
tty命令可以指寫輸入輸出的終端設備。如：tty /dev/ttyb
```


具體來說就是

例如一個程序名為prog 參數為 `-l a -C abc`
則，運行
```
gcc/g++ -g  prog.c/cpp -o prog
```

就可以用gdb調試程序prog
#gdb prog
進入gdb調試界面
輸入參數命令set args 後面加上程序所要用的參數，注意，不再帶有程序名，直接加參數，如：

```
set args -l a -C abc
```

回車後輸入
r
即可開始運行