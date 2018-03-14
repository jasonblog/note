---
title: C 加載過程
layout: post
comments: true
language: chinese
category: [linux,program]
keywords: linux,c,加載過程
description: 利用動態庫，可以節省磁盤、內存空間，而且可以提高程序運行效率；不過同時也導致調試比較困難，而且可能存在潛在的安全威脅。這裡主要討論符號的動態鏈接過程，即程序在執行過程中，對其中包含的一些未確定地址的符號進行重定位的過程。
---

利用動態庫，可以節省磁盤、內存空間，而且可以提高程序運行效率；不過同時也導致調試比較困難，而且可能存在潛在的安全威脅。

這裡主要討論符號的動態鏈接過程，即程序在執行過程中，對其中包含的一些未確定地址的符號進行重定位的過程。

<!-- more -->

## 簡介

`ld.so` (Dynamic Linker/Loader) 和 `ldd` 都會使用到 ELF 格式中的 `.dynstr` (dynamic linking string table) 字段，如果通過 `strip -R .dynstr hello` 命令將該字段刪除，那麼 `ldd` 就會報錯。

### Shell 執行

大致記錄一下 bash 的執行過程，當打開終端後，可以通過 `tty` 命令查看當前的虛擬終端，假設為 `/dev/pts/27`，然後再通過 `ps -ef | grep pts/27 | grep bash | grep -v grep` 查看對應的 PID 。

打開另一個終端，通過 `pstack PID` 即可看到對應的調用堆棧。

{% highlight text %}
main()                               ← 各種初始化工作，shell.c
 |-reader_loop()                     ← 死循環，通過read_command()讀取命令，eval.c
   |-read_command()                  ← 通過parse_command()和yyparse()解析輸入的命令，eval.c
   |-execute_command()               ← 開始執行命令，execute_cmd.c
     |-execute_command_internal()
       |-execute_simple_command()
         |-execute_disk_command()
           |-execve()                ← 通過系統調用執行
{% endhighlight %}

其中詞法語法解析通過 `flex-biso` 解析，涉及的文件為 `parse.y`，沒有找到詞法解析的文件。

{% highlight text %}
$ rpm -qf `which bash`                      // 查看所屬包
$ yumdownloader --source bash               // 下載源碼
$ rpm2cpio bash-version.src.rpm | cpio -id  // 解壓源碼
{% endhighlight %}

通過 `strace ./hello` 查看系統調用，定位到 `execve()` ，也就是通過該函數執行。

### 常見概念

解釋器 `.interp` 分區用於指定程序動態裝載、鏈接器 `ld-linux.so` 的位置，而過程鏈接表 `plt`、全局偏移表 `got`、重定位表則用於輔助動態鏈接過程。

#### 符號

對於可執行文件除了編譯器引入的一些符號外，主要就是用戶自定義的全局變量、函數等，而對於可重定位文件僅僅包含用戶自定義的一些符號。

{% highlight text %}
----- 生成可重定位文件，並通過nm命令查看ELF文件的符號表信息
$ gcc -c main.c
$ nm main.o
0000000000000000 B global
0000000000000000 T main
                 U printf
{% endhighlight %}

上面包含全局變量、自定義函數以及動態鏈接庫中的函數，但不包含局部變量，而且發現這三個符號的地址都沒有確定。

{% highlight text %}
----- 生成可執行文件
$ gcc -o main main.o
$ nm main | egrep "main$| printf|global$"
0000000000601038 B global
000000000040052d T main
                 U printf@@GLIBC_2.2.5
{% endhighlight %}

經鏈接之後，`global` 和 `main` 的地址都已經確定了，但是 `printf` 卻還沒，因為它是動態鏈接庫 `glibc` 中定義函數，需要動態鏈接，而不是這裡的靜態鏈接。

也就是說 main.o 中的符號地址沒有確定，而經過鏈接後部分符號地址已經確定，也就是對符號的引用變成了對地址的引用，這樣程序運行時就可通過訪問內存地址而訪問特定的數據。對於動態鏈接庫，也就是上述的 `printf()` 則需要在運行時通過動態鏈接器 ld-linux.so 進行重定位，即動態鏈接。

另外，除了 nm 還可以用 `readelf -s` 查看 `.dynsym` 表或者用 `objdump -tT` 查看。

{% highlight text %}
$ nm -D /lib64/libc-2.17.so | grep "\ printf$"
{% endhighlight %}

注意，在部分新系統上，如果不使用參數 `-D` ，那麼可能會無法查看符號表，因為 nm 默認打印 `.symtab` 和 `.strtab`，不過一般在打包時會通過 strip 刪除掉，只保留了動態符號 (在 `.dynsym` 和 `.dynstr` 中)，以便動態鏈接器在執行程序時尋址這些外部用到的符號。

<!-- https://stackoverflow.com/questions/9961473/nm-vs-readelf-s -->

<!--
## 動態鏈接

動態鏈接就是在程序運行時對符號進行重定位，確定符號對應的內存地址的過程。為了提高效率，Linux 下符號的動態鏈接默認採用 Lazy Mode 方式，也就是在程序運行過程中用到該符號時才去解析它的地址。

不過這種默認是可以通過設置 LD_BIND_NOW 為非空來打破的（下面會通過實例來分析這個變量的作用），也就是說如果設置了這個變量，動態鏈接器將在程序加載後和符號被使用之前就對這些符號的地址進行解析。

## 動態鏈接庫

在程序中，保存了依賴的庫信息。

$ readelf -d main | grep NEEDED
 0x0000000000000001 (NEEDED)             Shared library: [libc.so.6]

在 `.dynamic` 分區中保存了和動態鏈接相關的信息，當然只有版本信息，而沒有保存絕對路徑，其搜索路徑如上所述。

### FIXME: 獲取動態鏈接器

動態鏈接器保存在 `.interp` 分區，可以通過 `readelf -x .interp main` 命令查看。


注意，與 libc.so 之類的動態庫不同，ld-linux.so 的路徑是絕對路徑，而類似於 libc.so 庫則只包含了文件名。

這是因為，程序執行時 ld-linux.so 最先被加載到內存，沒有其他程序知道去哪裡查找 ld-linux.so，所以它的路徑必須是絕對的；當 ld-linux.so 被裝載以後，由它來去裝載可執行文件和相關的共享庫，它會根據下面介紹的流程去加載。

### 動態鏈接器

可以通過 `man ld-linux` 獲取與動態鏈接器相關的資料，包括各種相關的環境變量和文件都有詳細的說明。

對於環境變量，除了上面提到過的 LD_LIBRARY_PATH 和 LD_BIND_NOW 變量外，還有其他幾個重要參數，比如 LD_PRELOAD 用於指定預裝載一些庫，以便替換其他庫中的函數，從而做一些安全方面的處理 [6]，[9]，[12]，而環境變量 LD_DEBUG 可以用來進行動態鏈接的相關調試。
對於文件，除了上面提到的 ld.so.conf 和 ld.so.cache 外，還有一個文件 /etc/ld.so.preload 用於指定需要預裝載的庫。

實際上，ELF 格式可以從兩個角度去看，包括鏈接和執行，分別通過 `Section Header Table` 和 `Program Header Table` 表示。

SHT 保存了 ELF 所包含的段信息，可以通過 `readelf -S /bin/bash` 查看，其中比較重要的有 REL sections (relocations), SYMTAB/DYNSYM (symbol tables), VERSYM/VERDEF/VERNEED sections (symbol versioning information).

#### 1. 加載到內存

在 ELF 文件的文件頭中就指定了該文件的入口地址，程序的代碼和數據部分會相繼映射到對應的內存中。

$ readelf -h /bin/bash | grep Entry
  Entry point address:               0x41d361
-->


## 內核加載

ELF 有靜態和動態鏈接兩種方式，加載過程由內核開始，而動態鏈接庫的加載則可以在用戶層完成。GNU 對於動態鏈接過程為 A) 把 ELF 映像的裝入/啟動加載在 Linux 內核中；B) 把動態鏈接的實現放在用戶空間 (glibc)，併為此提供一個稱為 "解釋器" (ld-linux.so.2) 工具。

注意，解釋器的裝入/啟動也由內核負責，詳細可以查看 [內存-用戶空間](/post/kernel-memory-management-from-userspace-view.html) 中的介紹，在此只介紹 ELF 的加載過程。

### 內核模塊

如果要支持不同的執行格式，需要在內核中添加註冊模塊，每種類型通過 `struct linux_binfmt` 格式表示，其定義以及 ELF 的定義如下所示：

{% highlight c %}
struct linux_binfmt {
    struct list_head lh;
    struct module *module;
    int (*load_binary)(struct linux_binprm *);
    int (*load_shlib)(struct file *);
    int (*core_dump)(struct coredump_params *cprm);
    unsigned long min_coredump; /* minimal dump size */
};

static struct linux_binfmt elf_format = {
    .module        = THIS_MODULE,
    .load_binary   = load_elf_binary,
    .load_shlib    = load_elf_library,
    .core_dump     = elf_core_dump,
    .min_coredump  = ELF_EXEC_PAGESIZE,
};
{% endhighlight %}

其中的 `load_binary` 函數指針指向的就是一個可執行程序的處理函數，要支持 ELF 文件的運行，則必須通過 `register_binfmt()` 向內核登記這個數據結構，加入到內核支持的可執行程序的隊列中。

當要運行程序時，則掃描該隊列，讓各對象所提供的處理程序 (ELF中即為`load_elf_binary()`)，逐一前來認領，如果某個格式的處理程序發現相符後，便執行該格式映像的裝入和啟動。

### 內核加載

內核執行 `execv()` 或 `execve()` 系統調用時，會通過 `do_execve()` 調用，該函數先打開目標映像文件，並讀入文件的頭部信息，也就是開始 128 字節。

然後，調用另一個 `search_binary_handler()` 函數，該函數中會搜索上面提到的 Linux 支持的可執行文件類型隊列，讓各種可執行程序的處理程序前來認領和處理。

如果類型匹配，則調用 `load_binary` 函數指針所指向的處理函數來處理目標映像文件，對於 ELF 文件也就是 `load_elf_binary()` 函數，下面主要就是分析 `load_elf_binary()` 的執行過程。

{% highlight c %}
static int load_elf_binary(struct linux_binprm *bprm)
{
    /* 上述已經讀取 128 字節的頭部信息 */
    loc->elf_ex = *((struct elfhdr *)bprm->buf);

    retval = -ENOEXEC;
    /* 首先校驗ELF的頭部信息，也就是"\177ELF" */
    if (memcmp(loc->elf_ex.e_ident, ELFMAG, SELFMAG) != 0)
        goto out;
    /* 只能是可執行文件(ET_EXEC)或者動態庫(ET_DYN) */
    if (loc->elf_ex.e_type != ET_EXEC && loc->elf_ex.e_type != ET_DYN)
        goto out;

    /* 讀取ELF的頭部信息，也就是整個Program Header Table，一個可執行程序必須至少有一個段，且不能超過64K */
    retval = kernel_read(bprm->file, loc->elf_ex.e_phoff, (char *)elf_phdata, size);
    if (retval != size) {
        if (retval >= 0)
            retval = -EIO;
        goto out_free_ph;
    }

    /* 在for循環中，用來尋找和處理目標映像的"解釋器"段，可以通過 ???????? 查看 */
    for (i = 0; i < loc->elf_ex.e_phnum; i++) {
        if (elf_ppnt->p_type == PT_INTERP) {
            /* 找到後就根據其位置的p_offset和大小p_filesz把整個"解釋器"段的內容讀入緩衝區，實際上
             * 這個"解釋器"段實際上只是一個字符串，即解釋器的文件名，如"/lib/ld-linux.so.2"
             */
            retval = kernel_read(bprm->file, elf_ppnt->p_offset,
                         elf_interpreter, elf_ppnt->p_filesz);
            if (retval != elf_ppnt->p_filesz) {
                if (retval >= 0)
                    retval = -EIO;
                goto out_free_interp;
            }

            /* 通過open_exec()打開這個文件，再通過kernel_read()讀入開始的128個字節，也就是解釋器的頭部 */
            interpreter = open_exec(elf_interpreter);
            retval = PTR_ERR(interpreter);
            if (IS_ERR(interpreter))
                goto out_free_interp;
            retval = kernel_read(interpreter, 0, bprm->buf, BINPRM_BUF_SIZE);
            if (retval != BINPRM_BUF_SIZE) {
                if (retval >= 0)
                    retval = -EIO;
                goto out_free_dentry;
            }
            break;
        }
        elf_ppnt++;
    }

    /* 從目標映像的程序頭中搜索類型為PT_LOAD的段；在二進制映像中，只有類型為PT_LOAD的段才是需要加載的。
     * 只是在加載前，需要確定加載的地址，包括頁對齊、該段的p_vaddr域的值；在確定了裝入地址後，就會通過
     * elf_map()建立用戶空間虛擬地址空間與目標映像文件中某個連續區間之間的映射，其返回值就是實際映射的
     * 起始地址。
     */
    for(i = 0, elf_ppnt = elf_phdata; i < loc->elf_ex.e_phnum; i++, elf_ppnt++) {
        if (elf_ppnt->p_type != PT_LOAD)
            continue;

        error = elf_map(bprm->file, load_bias + vaddr, elf_ppnt, elf_prot, elf_flags, total_size);
        if (BAD_ADDR(error)) {
            retval = IS_ERR((void *)error) ?
                PTR_ERR((void*)error) : -EINVAL;
            goto out_free_dentry;
        }
    }

    /* 若存在PT_INTERP段，則通過load_elf_interp()加載，並把進入用戶空間的入口地址設置成load_elf_interp()
     * 的返回值，即解釋器映像的入口地址；如果不裝入解釋器，那麼這個入口地址就是目標映像本身的入口地址，
     * 也就是靜態編譯的程序。
     */
    if (elf_interpreter) {
        elf_entry = load_elf_interp(&loc->interp_elf_ex, interpreter, &interp_map_addr, load_bias);
        if (!IS_ERR((void *)elf_entry)) {
            interp_load_addr = elf_entry;
            elf_entry += loc->interp_elf_ex.e_entry;
        }
        if (BAD_ADDR(elf_entry)) {
            retval = IS_ERR((void *)elf_entry) ?  (int)elf_entry : -EINVAL;
            goto out_free_dentry;
        }
        reloc_func_desc = interp_load_addr;
    } else {
        elf_entry = loc->elf_ex.e_entry;
    }

    /* 完成加載且啟動用戶空間的映像運行之前，還需要為目標映像和解釋器準備好一些有關的信息，這些信息包括常
     * 規的argc、envc等等，還有一些"輔助向量(Auxiliary Vector)"。這些信息需要複製到用戶空間，使它們在CPU
     * 進入解釋器或目標映像的程序入口時出現在用戶空間堆棧上。這裡的create_elf_tables()就起著這個作用。
     */
    retval = create_elf_tables(bprm, &loc->elf_ex, load_addr, interp_load_addr);
    if (retval < 0)
        goto out;

    /* 最後，通過start_thread()宏將eip和esp改成新的地址，就使得CPU在返回用戶空間時就進入新的程序入口。如果存
     * 在解釋器映像，那麼這就是解釋器映像的程序入口(動態鏈接)，否則就是目標映像的程序入口(靜態鏈接)。
     */
    start_thread(regs, elf_entry, bprm->p);
}
{% endhighlight %}

## 加載過程

依賴動態庫時，會在加載時根據可執行文件的地址和動態庫的對應符號的地址推算出被調用函數的地址，這個過程被稱為動態鏈接。

假設，現在使用的是 Position Independent Code, PIC 模型。

#### 1. 獲取動態鏈接器

首先，讀取 ELF 頭部信息，解析出 `PT_INTERP` 信息，確定動態鏈接器的路徑，可以通過 `readelf -l foobar` 查看，一般是 `/lib/ld-linux.so.2` 或者 `/lib64/ld-linux-x86-64.so.2` 。

#### 2. 加載動態庫

關於加載的詳細順序可以查看 `man ld` 中 rpath-link 的介紹，一般順序為：

1. 鏈接時 `-rpath-link` 參數指定路徑，只用於鏈接時使用，編譯時通過 `-Wl,rpath-link=` 指定；
2. 鏈接時通過 `-rpath` 參數指定路徑，除了用於鏈接時使用，還會在運行時使用，編譯時可利用 `-Wl,rpath=` 指定，會生成 `DT_RPATH` 或者 `DT_RUNPATH` 定義，可以通過 `readelf -d main | grep -E (RPATH|RUNPATH)` 查看；
3. 查找 `DT_RUNPATH` 或者 `DT_RPATH` 指定的路徑，如果前者存在則忽略後者；
3. 依次查看 `LD_RUN_PATH` 和 `LD_LIBRARY_PATH` 環境變量指定路徑；
4. 查找默認路徑，一般是 `/lib` 和 `/usr/lib` ，然後是 `/etc/ld.so.conf` 文件中的配置。

另外，需要加載哪些庫通過 `DT_NEEDED` 字段來獲取，每條對應了一個動態庫，可以通過 `readelf -d main | grep NEEDED` 查看。

### 示例程序

利用如下的示例程序。

{% highlight c %}
/* filename: foobar.c */
#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>

int foobar(void)
{
    Dl_info dl_info;
    dladdr((void*)foobar, &dl_info);
    fprintf(stdout, "load .so at: %s\n", dl_info.dli_fname);
    return 0;
}
{% endhighlight %}

{% highlight c %}
/* filename: main.c */
int foobar(void);
int main(int argc, char **argv)
{
  foobar();
  return 0;
}
{% endhighlight %}

{% highlight makefile %}
# filename: Makefile
all:
    gcc --shared -fPIC foobar.c -o libfoobar.so -ldl
    gcc main.c -o main -Wl,-rpath-link=/foobar -ldl -lfoobar -L./
    readelf -d main | grep -E (RPATH|RUNPATH)
{% endhighlight %}

然後可以通過依次設置如上的加載路徑進行測試。**注意**，在對 `/etc/ld.so.conf` 文件設置後需要通過 `ldconfig` 更新 cache 才會生效。

另外，推薦使用 `DT_RUNPATH` 而非 `DT_RPATH` ，此時，在編譯時需要用到 `--enable-new-dtags` 參數。

<!--
通過C語言直接讀取rpath參數
#include <stdio.h>
#include <elf.h>
#include <link.h>

int main()
{
  const ElfW(Dyn) *dyn = _DYNAMIC;
  const ElfW(Dyn) *rpath = NULL;
  const char *strtab = NULL;
  for (; dyn->d_tag != DT_NULL; ++dyn) {
    if (dyn->d_tag == DT_RPATH) {
      rpath = dyn;
    } else if (dyn->d_tag == DT_STRTAB) {
      strtab = (const char *)dyn->d_un.d_val;
    }
  }

  if (strtab != NULL && rpath != NULL) {
    printf("RPATH: %s\n", strtab + rpath->d_un.d_val);
  }
  return 0;
}
-->

### 版本管理

不同版本的動態庫可能會不兼容，那麼如果程序在編譯時指定動態庫是某個低版本，運行是用的一個高版本，可能會導致無法運行。

假設有如下的示例：

{% highlight c %}
/* filename:hello.c */
#include <stdio.h>
void hello(const char* name)
{
    printf("hello %s!\n", name);
}
{% endhighlight %}

{% highlight c %}
/* filename:hello.h */
void hello(const char* name);
{% endhighlight %}

{% highlight makefile %}
# filename: Makefile
all:
    gcc hello.c -fPIC -shared -Wl,-soname,libhello.so.0 -o libhello.so.0.0.1
{% endhighlight %}

需要注意是，參數 `-Wl,soname` 中間沒有空格，`-Wl` 選項用來告訴編譯器將後面的參數傳遞給鏈接器，而 `-soname` 則指定了動態庫的 `soname`。運行後在當前目錄下會生成一個 `libhello.so.0.0.1` 文件，當運行 `ldconfig -n .` 命令時，當前目錄會多一個符號連接。

這個軟鏈接是根據編譯生成 `libhello.so.0.0.1` 時指定的 `-soname` 生成的，會保存到編譯生成的文件中，可以通過 `readelf -d foobar` 查看依賴的庫。

所以關鍵就是這個 soname，它相當於一箇中間者，當我們的動態庫只是升級一個小版本時，可以讓它的 soname 相同，而可執行程序只認 soname 指定的動態庫，這樣依賴這個動態庫的可執行程序不需重新編譯就能使用新版動態庫的特性。

#### 測試程序

示例程序如下。

{% highlight c %}
/* filename:main.c */
#include "hello.h"
int main()
{
    hello("foobar");
    return 0;
}
{% endhighlight %}

然後可以通過 `gcc main.c -L. -lhello -o main` 編譯，不過此時會報 `cannot find -lhello.so.0` 錯誤，也就是找不到對應的庫。

在 Linux 中，編譯時指定 `-lhello` 時，鏈接器會去查找 `libhello.so` 這樣的文件，如果當前目錄下沒有這個文件，那麼就會導致報錯；此時，可以通過 `ln -s libhello.so.0.0.1 libhello.so` 建立這樣一個軟鏈接。

通過 `ldd` 查看時，發現實際依賴的是 `libhello.so.0` 而非 `libhello` 也不是 `libhello.so.0.0.1` ，其實在生成 main 程序的過程有如下幾步：

1. 鏈接器通過編譯命令 `-L. -lhello` 在當前目錄查找 `libhello.so` 文件；
2. 讀取 `libhello.so` 鏈接指向的實際文件，這裡是 `libhello.so.0.0.1`；
3. 讀取 `libhello.so.0.0.1` 中的 `SONAME`，這裡是 `libhello.so.0`；
4. 將 `libhello.so.0` 記錄到 `main` 程序的二進制數據裡。

也就是說 `libhello.so.0` 是已經存儲到 main 程序的二進制數據裡的，不管這個程序在哪裡，通過 `ldd` 查看它依賴的動態庫都是 `libhello.so.0` 。

那麼，在部署時，只需要安裝 `libhello.so.0` 即可。

#### 版本更新

假設動態庫需要做一個小小的改動。

{% highlight c %}
/* filename:hello.c */
#include <stdio.h>
void hello(const char* name)
{
    printf("hello %s, welcom to our world!\n", name);
}
{% endhighlight %}

由於改動較小，編譯動態庫時仍然指定相同的 soname 。

{% highlight text %}
$ gcc hello.c -fPIC -shared -Wl,-soname,libhello.so.0 -o libhello.so.0.0.2
{% endhighlight %}

然後重新運行 `ldconfig -n .` 即可，會發現鏈接指向了新版本，然後直接運行即可。

同樣，假如我們的動態庫有大的改動，編譯動態庫時指定了新的 soname，如下：

{% highlight text %}
$ gcc hello.c -fPIC -shared -Wl,-soname,libhello.so.1 -o libhello.so.1.0.0
{% endhighlight %}

將動態庫文件拷貝到運行目錄，並執行 `ldconfig -n .`，不過此時需要重新編譯才可以。

## 動態解析

如上所述，控制權先是提交到解釋器，由解釋器加載動態庫，然後控制權才會到用戶程序。動態庫加載的大致過程就是將每一個依賴的動態庫都加載到內存，並形成一個鏈表，後面的符號解析過程主要就是在這個鏈表中搜索符號的定義。

<!--
對於靜態文件通常只有一個文件要被映射，而動態則還有所依賴的共享目標文件，通過 /proc/PID/maps 可以查看在內存中的分佈。
地址隨機實際上包括了動態鏈接庫、堆、棧，而對於程序本身的函數，其地址是固定的。
-->


{% highlight text %}
$ cat test.c
#include <stdio.h>
void foobar (void)
{
    puts("Hello World");
}

int main(void)
{
    foobar();
    return 0;
}

----- 編譯連接
$ gcc test.c -o test -g
----- 打印程序的反彙編
$ objdump -S test

----- 使用gdb調式
$ gdb test -q
(gdb) break main
(gdb) run
(gdb) disassemble
Dump of assembler code for function main:
   0x000000000040053d <+0>:     push   %rbp
   0x000000000040053e <+1>:     mov    %rsp,%rbp
=> 0x0000000000400541 <+4>:     callq  0x40052d <foobar>    此處調用的地址是固定的
   0x0000000000400546 <+9>:     mov    $0x0,%eax
   0x000000000040054b <+14>:    pop    %rbp
   0x000000000040054c <+15>:    retq   
End of assembler dump.
(gdb) disassemble foobar
Dump of assembler code for function foobar:
   0x000000000040052d <+0>:     push   %rbp
   0x000000000040052e <+1>:     mov    %rsp,%rbp
   0x0000000000400531 <+4>:     mov    $0x4005e0,%edi
   0x0000000000400536 <+9>:     callq  0x400410 <puts@plt>  反彙編
   0x000000000040053b <+14>:    pop    %rbp
   0x000000000040053c <+15>:    retq   
End of assembler dump.
{% endhighlight %}

從上面反彙編代碼可以看出，在調用 `foobar()` 時，使用的是絕對地址，`printf()` 的調用已經換成了 `puts()` ，調用的是 `puts@plt` 這個標號，位於 `0x400410`，實際上這是一個 PLT 條目，可以通過反彙編查看相應的代碼，不過它代表什麼意思呢？

在進一步說明符號的動態解析過程以前，需要先了解兩個概念，一個是 `Global Offset Table`，一個是 `Procedure Linkage Table` 。

#### Global Offset Table, GOT

在位置無關代碼中，如共享庫，一般不會包含絕對虛擬地址，而是在程序中引用某個共享庫中的符號時，編譯鏈接階段並不知道這個符號的具體位置，只有等到動態鏈接器將所需要的共享庫加載時進內存後，也就是在運行階段，符號的地址才會最終確定。

因此，需要有一個數據結構來保存符號的絕對地址，這就是 GOT 表的作用，GOT 表中每項保存程序中引用其它符號的絕對地址，這樣，程序就可以通過引用 GOT 表來獲得某個符號的地址。

<!--
在x86結構中，GOT表的前三項保留，用於保存特殊的數據結構地址，其它的各項保存符號的絕對地址。對於符號的動態解析過程，我們只需要瞭解的就是第二項和第三項，即GOT[1]和GOT[2]：GOT[1]保存的是一個地址，指向已經加載的共享庫的鏈表地址（前面提到加載的共享庫會形成一個鏈表）；GOT[2]保存的是一個函數的地址，定義如下：GOT[2] = &_dl_runtime_resolve，這個函數的主要作用就是找到某個符號的地址，並把它寫到與此符號相關的GOT項中，然後將控制轉移到目標函數，後面我們會詳細分析。
-->

#### Procedure Linkage Table, PLT

過程鏈接表的作用就是將位置無關的函數調用轉移到絕對地址。在編譯鏈接時，鏈接器並不能控制執行從一個可執行文件或者共享文件中轉移到另一箇中（如前所說，這時候函數的地址還不能確定），因此，鏈接器將控制轉移到PLT中的某一項。而PLT通過引用GOT表中的函數的絕對地址，來把控制轉移到實際的函數。

在實際的可執行程序或者共享目標文件中，GOT表在名稱為.got.plt的section中，PLT表在名稱為.plt的section中。


### PLT

在通過 `objdump -S test` 命令返彙編之後，其中的 `.plt` 內容如下。

{% highlight text %}
Disassembly of section .plt:

0000000000400400 <puts@plt-0x10>:
  400400:       ff 35 02 0c 20 00       pushq  0x200c02(%rip)        # 601008 <_GLOBAL_OFFSET_TABLE_+0x8>
  400406:       ff 25 04 0c 20 00       jmpq   *0x200c04(%rip)        # 601010 <_GLOBAL_OFFSET_TABLE_+0x10>
  40040c:       0f 1f 40 00             nopl   0x0(%rax)

0000000000400410 <puts@plt>:
  400410:       ff 25 02 0c 20 00       jmpq   *0x200c02(%rip)        # 601018 <_GLOBAL_OFFSET_TABLE_+0x18>
  400416:       68 00 00 00 00          pushq  $0x0
  40041b:       e9 e0 ff ff ff          jmpq   400400 <_init+0x20>

0000000000400420 <__libc_start_main@plt>:
  400420:       ff 25 fa 0b 20 00       jmpq   *0x200bfa(%rip)        # 601020 <_GLOBAL_OFFSET_TABLE_+0x20>
  400426:       68 01 00 00 00          pushq  $0x1
  40042b:       e9 d0 ff ff ff          jmpq   400400 <_init+0x20>

0000000000400430 <__gmon_start__@plt>:
  400430:       ff 25 f2 0b 20 00       jmpq   *0x200bf2(%rip)        # 601028 <_GLOBAL_OFFSET_TABLE_+0x28>
  400436:       68 02 00 00 00          pushq  $0x2
  40043b:       e9 c0 ff ff ff          jmpq   400400 <_init+0x20>

Disassembly of section .text:
{% endhighlight %}

當然，也可以通過 `gdb` 命令進行反彙編。

{% highlight text %}
(gdb) disassemble 0x400410
Dump of assembler code for function puts@plt:
   0x0000000000400410 <+0>:     jmpq   *0x200c02(%rip)        # 0x601018 <puts@got.plt>   查看對應內存
   0x0000000000400416 <+6>:     pushq  $0x0
   0x000000000040041b <+11>:    jmpq   0x400400
End of assembler dump.
{% endhighlight %}

可以看到 `puts@plt` 中包含三條指令，而且可以看出，除 `PLT0(__gmon_start__@plt-0x10)` 所標記的內容，其它的所有 `PLT` 項的形式都是一樣的，而且最後的 `jmp` 指令都是 `0x400400`，即 `PLT0` 為目標的；所不同的只是第一條 `jmp` 指令的目標和 `push` 指令中的數據。

`PLT0` 則與之不同，但是包括 `PLT0` 在內的每個表項都佔 16 個字節，所以整個 PLT 就像個數組。

另外，需要注意，每個 PLT 表項中的第一條 `jmp` 指令是間接尋址的，比如的 `puts()` 函數是以地址 `0x601018` 處的內容為目標地址進行中跳轉的。

### GOT

{% highlight text %}
----- 實際等價於jmpq *0x601018 ，而*0x601018就是0x00400416，就是會調轉到0x400416所在的地址執行，
----- 實際是順序執行，最終會調轉到0x400400
(gdb) x/w 0x601018
0x601018 <puts@got.plt>:        0x00400416

(gdb) x /3i 0x400400            查看反彙編
   0x400400:    pushq  0x200c02(%rip)         # 0x601008
   0x400406:    jmpq   *0x200c04(%rip)        # 0x601010   跟蹤進入
   0x40040c:    nopl   0x0(%rax)

(gdb) b *0x400406               設置斷點
(gdb) c
Breakpoint 2, 0x0000000000400406 in ?? ()
(gdb) ni
_dl_runtime_resolve () at ../sysdeps/x86_64/dl-trampoline.S:58
58              subq $REGISTER_SAVE_AREA,%rsp
(gdb) i r rip
rip            0x7ffff7df0290   0x7ffff7df0290 <_dl_runtime_resolve>
{% endhighlight %}

從上面可以看出，這個地址實際上就是順序執行，也就是 `puts@plt` 中的第二條指令，不過正常來說這裡應該保存的是 `puts()` 函數的地址才對，那為什麼會這樣呢？

<!--這裡的功能就是 Lazy Load，也就是延遲加載，只有在需要的時候才會加載。-->原來鏈接器在把所需要的共享庫加載進內存後，並沒有把共享庫中的函數的地址寫到 GOT 表項中，而是延遲到函數的第一次調用時，才會對函數的地址進行定位。

如上，在 `jmpq` 中設置一個斷點，觀察到，實際調轉到了 `_dl_runtime_resolve()` 這個函數。

### 地址解析

在 gdb 中，可以通過 `disassemble _dl_runtime_resolve` 查看該函數的反彙編，感興趣的話可以看看其調用流程，這裡簡單介紹其功能。

從調用 `puts@plt` 到 `_dl_runtime_resolve` ，總共有兩次壓棧操作，一次是 `pushq  $0x0`，另外一次是 `pushq  0x200c02(%rip) # 601008`，分別表示了 `puts` 函數在 `GOT` 中的偏移以及 `GOT` 的起始地址。

在 `_dl_runtime_resolve()` 函數中，會解析到 `puts()` 函數的絕對地址，並保存到 `GOT` 相應的地址處，這樣後續調用時則會直接調用 `puts()` 函數，而不用再次解析。

![elf load]({{ site.url }}/images/linux/elf-load-process.png "elf load"){: .pull-center }

上圖中的紅線是解析過程，藍線則是後面的調用流程。


## 參考

關於動態庫的加載過程，可以參考 [動態符號鏈接的細節](https://github.com/tinyclub/open-c-book/blob/master/zh/chapters/02-chapter4.markdown)。

<!--
reference/linux/linux-c-dynamic-loading.markdown

ELF文件的加載和動態鏈接過程
http://jzhihui.iteye.com/blog/1447570

linux是如何加載ELF格式的文件的，包括patchelf
http://cn.windyland.me/2014/10/24/how-to-load-a-elf-file/


readelf -h /usr/bin/uptime
可以找到 Entry point address ，也即程序的入口地址。

http://michalmalik.github.io/elf-dynamic-segment-struggles
https://greek0.net/elf.html
https://lwn.net/Articles/631631/
https://www.haiku-os.org/blog/lucian/2010-07-08_anatomy_elf/
 -->

{% highlight text %}
{% endhighlight %}
