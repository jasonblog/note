# Linux添加環境變量與GCC編譯器添加INCLUDE與LIB環境變量


對所有用戶有效在`/etc/profile`增加以下內容。只對當前用戶有效在Home目錄下的
`.bashrc`或`.bash_profile`裡增加下面的內容：
(注意：等號前面不要加空格,否則可能出現 command not found)

#在PATH中找到可執行文件程序的路徑。

```sh
export PATH =$PATH:$HOME/bin
```

#gcc找到頭文件的路徑
```sh
C_INCLUDE_PATH=/usr/include/libxml2:/MyLib
export C_INCLUDE_PATH
```

#g++找到頭文件的路徑
```sh
CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:/usr/include/libxml2:/MyLib
export CPLUS_INCLUDE_PATH
```

#找到動態鏈接庫的路徑
```sh
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/MyLib
export LD_LIBRARY_PATH
```

#找到靜態庫的路徑
```sh
LIBRARY_PATH=$LIBRARY_PATH:/MyLib
export LIBRARY_PATH
```


庫 文件在連接（靜態庫和共享 庫）和運行（僅限於使用共享庫的程序）時被使用，其搜索路徑是在系統中進行設置的。一般 Linux 系統把 /lib 和 /usr/lib 兩個目錄作為默認的庫搜索路徑，所以使用這兩個目錄中的庫時不需要進行設置搜索路徑即可直接使用。對於處於默認庫搜索路徑之外的庫，需要將庫的位置添加到 庫的搜索路徑之中。設置庫文件的搜索路徑有下列兩種方式，可任選其一使用：

在環境變量 LD_LIBRARY_PATH 中指明庫的搜索路徑。

在 /etc/ld.so.conf 文件中添加庫的搜索路徑。

將自己可能存放庫文件的路徑都加入到/etc/ld.so.conf中是明智的選擇

添加方法也極其簡單，將庫文件的絕對路徑直接寫進去就OK了，一行一個。例如：

```sh
/usr/X11R6/lib

/usr/local/lib

/opt/lib
```

需 要注意的是：第二種搜索路徑的設置方式對於程序連接時的庫（包括共享庫和靜態庫） 的定位已經足夠了，但是對於使用了共享庫的程序的執行還是不夠的。這是因為為了加快程序執行時對共享庫的定位速度，避免使用搜索路徑查找共享庫的低效率， 所以是直接讀取庫列表文件 /etc/ld.so.cache 從中進行搜索的。/etc/ld.so.cache 是一個非文本的數據文件，不能直接編輯，它是根據 /etc/ld.so.conf 中設置的搜索路徑由 /sbin/ldconfig 命令將這些搜索路徑下的共享庫文件集中在一起而生成的（ldconfig 命令要以 root 權限執行）。因此，為了保證程序執行時對庫的定位，在 /etc/ld.so.conf 中進行了庫搜索路徑的設置之後，還必須要運行 /sbin/ldconfig 命令更新 /etc/ld.so.cache 文件之後才可以。ldconfig ,簡單的說，它的作用就是將/etc/ld.so.conf列出的路徑下的庫文件緩存到/etc/ld.so.cache 以供使用。因此當安裝完一些庫文件，(例如剛安裝好glib)，或者修改ld.so.conf增加新的庫路徑後，需要運行一下 /sbin/ldconfig使所有的庫文件都被緩存到ld.so.cache中，如果沒做，即使庫文件明明就在/usr/lib下的，也是不會被使用 的，結果編譯過程中抱錯，缺少xxx庫，去查看發現明明就在那放著，搞的想大罵computer蠢豬一個。

在程序連接時，對於庫文件（靜態庫和共享庫）的搜索路徑，除了上面的設置方式之外，還可以通過 -L 參數顯式指定。因為用 -L 設置的路徑將被優先搜索，所以在連接的時候通常都會以這種方式直接指定要連接的庫的路徑。

前 面已經說明過了，庫搜索路徑的設置有兩種方式：在環境變量 LD_LIBRARY_PATH 中設置以及在 /etc/ld.so.conf 文件中設置。其中，第二種設置方式需要 root 權限，以改變 /etc/ld.so.conf 文件並執行 /sbin/ldconfig 命令。而且，當系統重新啟動後，所有的基於 GTK2 的程序在運行時都將使用新安裝的 GTK+ 庫。不幸的是，由於 GTK+ 版本的改變，這有時會給應用程序帶來兼容性的問題，造成某些程序運行不正常。為了避免出現上面的這些情況，在 GTK+ 及其依賴庫的安裝過程中對於庫的搜索路徑的設置將採用第一種方式進行。這種設置方式不需要 root 權限，設置也簡單：

```sh
$ export LD_LIBRARY_PATH=/opt/gtk/lib:$LD_LIBRARY_PATH
```

可以用下面的命令查看 LD_LIBRAY_PATH 的設置內容：

```sh
$ echo $LD_LIBRARY_PATH
```

至此，庫的兩種設置就完成了。