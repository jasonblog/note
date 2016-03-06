# LIBRARY_PATH和LD_LIBRARY_PATH環境變量的區別


LIBRARY_PATH和LD_LIBRARY_PATH是Linux下的兩個環境變量，二者的含義和作用分別如下：

LIBRARY_PATH環境變量用於在程序編譯期間查找動態鏈接庫時指定查找共享庫的路徑，例如，指定gcc編譯需要用到的動態鏈接庫的目錄。設置方法如下（其中，LIBDIR1和LIBDIR2為兩個庫目錄）：

```sh
export LIBRARY_PATH=LIBDIR1:LIBDIR2:$LIBRARY_PATH
```

LD_LIBRARY_PATH環境變量用於在程序加載運行期間查找動態鏈接庫時指定除了系統默認路徑之外的其他路徑，注意，LD_LIBRARY_PATH中指定的路徑會在系統默認路徑之前進行查找。設置方法如下（其中，LIBDIR1和LIBDIR2為兩個庫目錄）：
```sh
export LD_LIBRARY_PATH=LIBDIR1:LIBDIR2:$LD_LIBRARY_PATH
```

舉個例子，我們開發一個程序，經常會需要使用某個或某些動態鏈接庫，為了保證程序的可移植性，可以先將這些編譯好的動態鏈接庫放在自己指定的目錄下，然後按照上述方式將這些目錄加入到LD_LIBRARY_PATH環境變量中，這樣自己的程序就可以動態鏈接後加載庫文件運行了。
區別與使用：

 

 

`開發時`，設置LIBRARY_PATH，以便gcc能夠找到`編譯時`需要的動態鏈接庫。

`發佈時`，設置LD_LIBRARY_PATH，以便`程序加載運行時`能夠自動找到需要的動態鏈接庫。