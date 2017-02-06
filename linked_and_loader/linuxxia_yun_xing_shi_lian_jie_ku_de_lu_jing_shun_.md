# linux下運行時連結庫的路徑順序


連結時路徑（Link-time path）和運行時路徑（Run-time path）不是一回事，當然，當你知道這個概念的時候，你當然明白。

1`.編譯目標代碼時指定的動態庫搜索路徑(指的是用-wl,rpath或-R選項而不是-L)；`

example: gcc -Wl,-rpath,/home/arc/test,-rpath,/lib/,-rpath,/usr/lib/,-rpath,/usr/local/lib test.c

2.環境變量LD_LIBRARY_PATH指定的動態庫搜索路徑；（export LD_LIBRARY_PATH=/root/test/env/lib）

3.配置文件/etc/ld.so.conf中指定的動態庫搜索路徑；（更改/etc/ld.so.conf文件後記得一定要執行命令：ldconfig！該命令會將/etc/ld.so.conf文件中所有路徑下的庫載入內存中）

4.默認的動態庫搜索路徑/lib；


5.默認的動態庫搜索路徑/usr/lib。


在命令前加環境變量設置，該環境變量只對該命令有效，當該命令執行完成後，該環境變量就無效了。如下述命令：# `LD_LIBRARY_PATH=/root/test/env/lib ./pos`

下面對`編譯`時庫的查找與運行時庫的查找做一個簡單的比較：

1. `編譯時查找的是靜態庫或動態庫，而運行時，查找的只是動態庫。`

2. 編譯時可以用-L指定查找路徑，或者用環境變量LIBRARY_PATH，而運行時可以用-Wl,rpath或-R選項，或者修改/etc/ld.so.conf文件或者設置環境變量LD_LIBRARY_PATH.

3. `編譯時用的連結器是ld`，`而運行時用的連結器是/lib/ld-linux.so.2`.

4. 編譯時與運行時都會查找默認路徑：/lib /usr/lib

5. 編譯時還有一個默認路徑：/usr/local/lib，而運行時不會默認找查該路徑。

原文網址：https://read01.com/D58naQ.html