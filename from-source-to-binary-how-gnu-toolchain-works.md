# 筆記：from Source to Binary: How GNU Toolchain Works


- 筆記

    - 微言大意：一個單純的描述背後隱含大量的細節和操作
        - printf放在哪裡，怎麼被系統呼叫？
        - main()函數怎麼被呼叫？
        - \#include到底做了什麼事？

    - 傳統程式碼從Compile到執行 階段每段的最佳化都和上下階段無關，上面的資訊下面的無法存取
        - Compiler 知道原始碼的結構，但是沒有變數和函數怎麼放置的資訊
        - Linker 和Compiler相反；知道變數和函數怎麼放置，但是沒有原始碼的結構的資訊
    - 一個source code從編譯到產生binary使用到的GNU toolchain
        - cpp: 處理巨集產生*.i檔案
        - cc1: 產生*.s (組合語言)檔案
        - as: 產生*.o 檔案
        - collect2: 沒放在PATH內，Ubuntu 12.04 放在/lib/gcc/x86_64-linux-gnu/4.6/collect2。Wrap linker and generate constructor code if needed.
        - ld: 吃link script，library, C runtime以及*.o，產生binary *
    - gcc
        - GNU compiler collection
        - compiler driver
        - compile時幫使用者呼叫cpp, cc1, as ....等程式並處理對應的參數
    - Intermediate Representation在gcc也有不同的leve
        - High Level : GENERIC (Syntax Tree Style IR)
        - Middle Level : Gimple (Tree Style IR, SSA form)
        - Low Level : RTL (List Style IR, Register Based)
    - SSA: Static Single Assignment
        - 每次的assign expression將變數加上版本號碼
            - a = b + 1; a++ ; b++; 變成
            - a1 = b1 + 1; a2++; b2++;
        - 如果變數assign expression是條件式的結果，使用Φ函數從集合中選擇
            - if (cond) { a = 1} else { a = 2 }變成
            - if (cond1) { a1 = 1} else { a2 = 2 } => a3 = Φ (a1, a2)
        - SSA的優點 (pass，每個應該是大哉問)
    - GCC最佳化
        - 將程式碼轉成語法樹
        - 把語法樹使用binary operation方式呈現，接著轉成SSA並針對SSA最佳化
        - 將SSA轉成RTL，使用和平臺相關的最佳化如pipeline最佳化、針對平臺最佳化過的函數、找出可以簡化的機械碼片段取換等
           - Peephole optimization
           - Other Built-in Functions Provided by GCC
           - Instruction scheduling
    - as的任務
        - 產生obj 檔案
        - 讓linker resolve symbol: ex: test.c 呼叫了printf("hello\n");，printf在哪裡？
            - 產生symbol table
            - 產生relocation table
    - symbol，用來協助linker找到或連結變數或是函數正確的位址
        - 先標記檔案內的變數或函數是internal的還是external的
    - binary分類
        - obj file
            - 不需要處理symbol和relocation問題
        - 執行檔: linker要把需要的function或變數從*.o檔案和在一起，或是動態使用symbol。
            - static link
                - 要處理symbol和relocation問題
            - dynamic link
                - 要處理symbol和relocation問題
                - 要處理動態呼叫library的問題
        - shared library * 要處理symbol和relocation問題
            - 要處理動態呼叫library的問題
            - 使用Position independent code
                - 載入時才確認address
                - global variable使用base + offset方式
                - 會有overhead
    - dynamic linker
        - 優點
            - 不同process可共用dynamic library 可共用的部份如程式碼（應該要thread safe?），想像不同process呼叫printf，而printf程式碼只需要一份放在shared library-> 節省記憶體
            - 系統只放一份shared library -> 節省空間
            - 更新library不需要重邊執行檔
            - Load on demand
        - 執行檔dynamic Link 步驟
            - Compile time
                - ld -> 從shared linker取得relocation和symbol資訊，並將資訊放到binary執行檔內
            - runtime:
                - 使用者執行程式
                - 系統從程式中取得dynamic linker路徑。
                - dynamic linker會把shared library相關的檔案如libc.so的.data和.text relocate到記憶體中
                - resolve相關的symbol
                - 開始執行程式
        - Unix世界中，dynamic linker屬於C library
        
        
```c
$ ls /lib64/ld-linux-x86-64.so.2  -
ld-linux-x86-64.so.2 -> /lib/x86_64-linux-gnu/ld-2.15.so*

$ file /lib/x86_64-linux-gnu/ld-2.15.so
/lib/x86_64-linux-gnu/ld-2.15.so: ELF 64-bit LSB shared object, x86-64, version 1 (SYSV), dynamically linked, BuildID[sha1]=0x930bb48366d22fbd8e002ef1c09f3061a506b43e, stripped

```
