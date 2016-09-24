# cppcheck 靜態程式分析工具


cppcheck是一套跨平台的靜態程式分析工具(static code analysis tool)，主要用來分析C語言在coding上的問題，包括memory leak、null pointer、buffer overflow這些算是bug的問題，也可檢查出多餘的程式碼達到增進程式效能的目的，例如沒使用到的變數、給定初始值的參數但從未使用過此初始值、不可能進入的condition等等

##使用cppcheck
首先必須安裝cppcheck
```sh
sudo apt-get install cppcheck
```

使用時輸入以下指令，最後的enable=all代表所有的檢查都執行，包含coding上的問題、邏輯上的問題、效能上的增進等等

```sh
cppcheck xxx.c --enable=all
```

因cppcheck default會作許多preprocessor的檢查，這部分會耗費相當長的時間，若不需要可將其關閉

```sh
cppcheck -DA xxx.c --enable=all
```

接著我們看一下一些都可編譯過但事實上是有問題的例子，看cppcheck是否可檢查出來


##變數宣告
觀察下段程式碼，我們來看一下cppcheck可檢查出哪些問題

```c
int a;
int b;
int c = 1;
int d = 2;

b++;
c++;
d = 4;
```

```sh
(style) Variable 'd' is reassigned a value before the old one has been used.
```

d被初始化成2但是還沒使用過前就被改掉了，因此賦予初始值是多餘的動作

```sh
(style) Unused variable: a
```

a被宣告但從未使用過，此宣告可以直接拿掉

```sh
(style) Variable 'b' is not assigned a value.
```

b需要給初始值，因在後面的程式有作b++，若不給予初始值會是未定義的數值+1(local變數在ANSI C裡並沒有規定default值需為0，事實上對local variable賦予初始值對CPU來說也是一項負擔)

```sh
Variable 'd' is assigned a value that is never used.
```

d被賦予初始值，但從未使用過

```sh
(error) Uninitialized variable: b
```

和第三項相呼應，作b++但未給予初始值

##記憶體管理
記憶體配置在寫程式上是非常好用的技巧，可用精簡的方式完成許多複雜的事情，但在C的架構上部分的記憶體管理需由程式設計師自行負責，若使用不當容易造成run time error

```c
int *ptra;
int *ptrb;
int *ptrc;
int *ptrd = NULL;
    
ptra = (int*)malloc(100);
ptrb = (int*)malloc(100);
    
if (NULL == ptra) {
	return *ptrd;
}
    
*(ptrc + 1) = 5;
    
free(ptrb);
    
*(ptrb + 2) = 6;
```

```sh
(error) Null pointer dereference: ptrd
```

ptrd在程式執行過程中有回傳值，但卻沒有配置參考的位址

```sh
(style) Variable 'ptrc' is not assigned a value.
```

ptrc過程中有被指定數值，但卻沒有配置參考的位址

```sh
(error) Memory leak: ptrb
(error) Memory leak: ptra
```

ptra有malloc卻沒有free，ptrb最後有free，但在if(NULL == ptra)裡面沒有free，也有機會造成memory leak
```sh
(error) Dereferencing 'ptrb' after it is deallocated / released
```

ptrb在free之後還對其操作

```sh
(error) Null pointer dereference
```

和第一項相呼應，回傳位配置空間的位址的值

```sh
(error) Uninitialized variable: ptrc
```

和第二項相呼應，ptrc沒有配置空間

##Buffer邊界
C的架構上對於buffer的overwrite是不會產生任何錯誤或警告訊息的

```c
int i;
int a[10];
    
for (i = 0; i <= 10; i++)
	a[i] = i;
```

```sh
(error) Array 'a[10]' accessed at index 10, which is out of bounds.
```


迴圈造成記憶體被overwrite

目前市面上的靜態程式檢查工具眾多，例如splint, coverity, cppcheck等等，每個工具都有其擅長的部分
就使用過splint和cppcheck的經驗來看例如對於不需要使用或不需要給初始值的狀況cppcheck > splint，但對於buffer overwrite的檢查splint > cppchek，因此若想讓程式更加完善，可以使用多些檢查工具達成目標

最後分享一下書上看到的

>>Lint Early, Lint Often

>>Lint is your software conscience. It tells you when you are doing bad things. Always use lint. Listen to your conscience.