# Linux 動態連結資料庫



我想要了解linux裡面是如何去編譯聯結的, 因此找到這一篇文章 http://www.suse.url.tw/sles10/lesson7.htm#26

Linux裡面資料庫連結方是有兩種, 一個是靜態連結(xxx.a), 另一種是動態連結(xxx.so)~ 這兩種各有優缺點,
靜態函式庫 (Static Libraries)會將整個程式碼編譯入主程式裡面, 所以size會比較大, 但也因為如此該程式可以單獨運行
動態函式庫 (通常為 xx.so，如您看到的是 xx.so.2.1 之類的，那個 2.1 就是版本囉 ) 則不需整個被編譯到主程式裡邊，
只需在程式的原始碼裡設計去呼叫動態函式庫即可。在 Linux 中所執行的程式幾乎都是使用動態連結資料庫。

提供系統來搜尋的函式庫，其所放置的目錄是定義在` /etc/ld.so.conf` 裡,
一般傳統的函式庫是安置在` /lib 及 /usr/lib` 目錄中

當執行 `ldconfig` 指令時，會先去讀取 `/etc/ld.so.conf`，以找出函式庫所放置的目錄位置，
然後才能將這些函式庫資料寫入 `cache` 內，並存放一份在` /etc/ld.so.cache` 中。




### 而我們在執行一支程式時，就會由` ld.so 這個動態連結器 (dynamic linker) 程式`，到 `cache` 內去尋找執行程式所需的函式，以提供程式做動態連結。

不過您要是看過 /etc/ld.so.conf，會發現 /lib 及 /usr/lib 並沒有定義在 ld.so.conf 內，
因為這兩個目錄是`內定值`，所以不需要做設定啦 !

最後如果您有設定 LD_LIBRARY_PATH 這個環境變數的話，其所定義的目錄也會被讀入。

如果哪天您函式庫所存放的目錄有所異動，請記得修改 /etc/ld.so.conf，然後再執行 ldconfig 指令，這樣才可以更新 cache 內的資料。以下我們稍作整理：



- DLL 一般是安置在 /lib 或 /usr/lib 目錄下，而除了這兩個目錄外，DLL 所存放的目錄必須在 /etc/ld.so.conf 或 LD_LIBRARY_PATH 中做定義。

- 執行指令 ldconfig 時，會先去讀取 /etc/ld.so.conf，然後將 DLL 讀入 cache，並紀錄一份在 /etc/ld.so.cache。當執行程式在執行時，ld.so 就會根據 cache 來找出並載入程式所需的函式庫，以讓程式能正確的執行。

- 欲瀏覽 /etc/ld.so.cache 內的資料，可執行：
- suse:~ # ldconfig -p
- 增加、減少、刪除部份 DLL 或 DLL 目錄有異動，就須執行 ldconfig，以更新 cache 的資料。
- 系統於開機時，預設就會執行 ldconfig，以確保 cache 內的資料是最新的。


至於要查詢所執行的程式使用了哪些 DLL，則執行 ldd 指令即可：


```sh
# ldd /bin/ls
linux-vdso.so.1 => (0x00007fff61f88000)
libselinux.so.1 => /lib/libselinux.so.1 (0x00007fc205320000)
librt.so.1 => /lib/librt.so.1 (0x00007fc205118000)
libacl.so.1 => /lib/libacl.so.1 (0x00007fc204f0f000)
libc.so.6 => /lib/libc.so.6 (0x00007fc204b8c000)
libdl.so.2 => /lib/libdl.so.2 (0x00007fc204988000)
/lib64/ld-linux-x86-64.so.2 (0x00007fc205559000)
libpthread.so.0 => /lib/libpthread.so.0 (0x00007fc20476a000)
libattr.so.1 => /lib/libattr.so.1 (0x00007fc204565000)
```

鳥哥: http://linux.vbird.org/linux_basic/0520source_code_and_tarball.php#library

首先，我們必須要在 /etc/ld.so.conf 裡面寫下『 想要讀入快取記憶體當中的動態函式庫所在的目錄』，注意喔， 是目錄而不是檔案；
接下來則是利用 ldconfig 這個執行檔將 /etc/ld.so.conf 的資料讀入快取當中；
同時也將資料記錄一份在 /etc/ld.so.cache 這個檔案當中吶！



圖 5.2.1、使用 ldconfig 預載入動態函式庫到記憶體中

