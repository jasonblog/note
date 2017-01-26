# 動態鏈接庫的搜索路徑


man ld.so(8)說，如果庫依賴不包括“/”，那麼它將按照下面的規則按順序搜索：

- （僅對ELF格式）如果可執行文件包含DT_RPATH標籤，並且不包含DT_RUNPATH標籤，將從DT_RPATH列出的路徑搜索。（DT_RPATH已經被廢棄，請用DT_RUNPATH）
- 如果LD_LIBRARY_PATH在程序運行時被定義，那麼將從它包含的路徑開始。安全起見，對於set-user-ID或者set-group-ID的程序，這個變量將被忽略。
- （僅對ELF格式）如果可執行文件包含DT_RUNPATH標籤，將從這個標籤列出的路徑開始搜索。
- 從 /etc/ld.so.cache（運行ldconfig產生）中查找文件
- 從/lib以及/urs/lib，按順序搜索。如果鏈接時指定-z nodefaultlib,這個步驟將被忽略。

看起來夠簡潔的，當做休閒，寫個程序驗證一下。但在這之前，先介紹一個Glibc擴展的函數（POSIX中沒有）

```c
#define _GNU_SOURCE
#include <dlfcn.h>

int dladdr(void* addr, Dl_info *info);
```

這個函數解析傳入的函數指針（第一個參數），將信息填充到Dl_info的結構體

```c
typedef struct {
    const char *dli_fname;  /* Pathname of shared object that contains address */
    void       *dli_fbase;  /* Address at which shared object  is loaded */
    const char *dli_sname;  /* Name of nearest symbol with addresslower than addr */
    void       *dli_saddr;  /* Exact address of symbol named in dli_sname */
} Dl_info;
```


下面是程序以及需要加載的動態庫的代碼：

- ld_main.c

```c
int main()
{
	lib_fun();
	return 0;
}
```

- ld_lib.c

```c
#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
int lib_fun()
{
	Dl_info dl_info;
	dladdr((void*)lib_fun, &dl_info);
	fprintf(stdout, ".so@ %s.\n", dl_info.dli_fname);
	return 0;
}
```
編譯這兩個文件：
```sh
1、動態庫：gcc --shared -fPIC ld_lib.c -o libld_lib.so -ldl
2、主程序：gcc ld_main.c -o ld_main -Wl,-rpath,./  -ldl -lld_lib -L./
```
-Wl,-rpath編譯選項將在程序中生成DT_RPATH節點，使用readelf會看到Library rpath被設為當前目錄：

![](./images/20131105101102375)

接下來將生成的libld_lib.so拷貝到前面介紹到的搜索路徑:<br>
對於LD_LIBRARY_PATH，隨便設置：export LD_LIBRARY_PATH=../<br>
對於ld.so.conf提到的路徑，在/etc/ld.so.conf.d/下面隨便找一個，或者自己建立一個，這裡用系統自帶的libc.conf<br>
中提到的路徑：/usr/local/lib<br>

![](./images/20131105102103015)

然後運行（每次都刪除程序優先加載的so文件）：

![](./images/20131105102749656)

（ld.so.conf路徑更新文件後需要運行ldconfig更新cache，否則會找不到文件，如上圖）。

關於-z nodefaultlib鏈接選項：


![](./images/20131105103331156)

看來它真起作用了
關於DT_RUNPATH，需要用到--enable-new-dtags鏈接選項：

![](./images/20131105103810531)

（`Linux下程序默認不會從當前路徑搜索.so文件`，這對於自行開發的分為很多模塊，要安裝在同一目錄的“程序”來說不是個優點。還好可以用DT_RUNPATH指定.so的加載路徑）