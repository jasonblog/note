# 目的檔

## 目的檔格式

可執行檔格式(Executable)

```sh
Windows : PE  (Portable Executable)
Linux   : ELF (Executable Linkable Format)
Intel   : OMF (Object Module Format)
Unix    : .out
DOS     : .COM
```

以上都是 COFF(Common object file format) 格式的變化

動態連結程式庫

```sh
Windows : .dll
Linux   : .so
```

靜態連結程式庫

```sh
Windows : .lib
Linux   : .a
```


動態/靜態連結程式庫也是按照可執行檔格式儲存

##ELF 格式

```sh
Relocatable     File : 程式碼和資料，可連結成可執行檔(.o/.obj)
Executable      File : 可執行的程式(/bin/bash/.exe)
Shared Object   File : 可變為目的檔或連結其他共用目的檔和可執行檔(.so/.dll)
Core Dump       File : 當行程意外終止時，可將行程資訊 dump(core dump)
```

在 Linux 可用 file 指令看檔案格式

```sh
$ file hello.c
hello.c: C source, ASCII text

$ file hello.i
hello.i: C source, ASCII text

$ file hello.o  //目的檔也是可執行檔格式(relocatable)
hello.o: ELF 64-bit LSB  relocatable, x86-64, version 1 (SYSV), not stripped

$ file hello.s
hello.s: assembler source text

$ file /bin/bash  //可執行檔格式(executable)
/bin/bash: ELF 64-bit LSB  executable, x86-64, version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 2.6.24

$ file /lib32/ld-2.19.so  //動態連結程式庫(shared object)
/lib32/ld-2.19.so: ELF 32-bit LSB  shared object, Intel 80386, version 1 (SYSV), dynamically linked
```

## 目的檔內容

目的檔不外乎儲存編譯後的機器碼指令

還有連結需要的資訊(符號表、除錯資訊)

這些不同資訊按照屬性存在 Section 或稱為 Segment 以下稱段

機器碼放在程式碼區段，此段稱為 `.code 或 .text`

`全域變數和區域靜態變數`在資料區段，稱為`.data`

```c
int g_init_var  = 84;     // .data
int g_uninit_var;         // .bss

void func1(int i)         ---+
{                            |
    printf("%d\n",i);        |--> .text
}                            | 
                             |
int main(void)               |
{                         ---+
    static int static_var1 = 85;  // .data
    static int static_var2;       // .bss
     
    int a = 1;                    ---+
    int b;                           |
    func1(static_var1 +              |
          static_var2 + a + b);      |--> .text
                                     |
    return 0;                        |
}                                 ---+
```

Executable File / Object File

```sh
// 分成 4 個區塊
File Header
.text
.data
.bss
```

目的檔可用 objdump 看

