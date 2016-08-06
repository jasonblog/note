# 使用strace找出程式缺少的檔案路徑


這算極短篇。在組裝別人軟體的時候，常常出現缺少檔案的錯誤，運氣不好的不會跟你說缺少的檔案的期待路徑；運氣更差的就會連錯誤都不印，直接程式crash。我後來知道strace之後，才發覺它可以結省很多印log和trace程式碼的時間。

這次就以前一篇執行openocd遇到的問題為例：第一次編譯openocd後，直接執行會出現找不到openocd.cfg檔案。經過一些試誤後才有上一篇整理出來的指令。

錯誤訊息如下

```
$ openocd 
Open On-Chip Debugger 0.10.0-dev-00250-g9c37747 (2016-04-07-22:20)
Licensed under GNU GPL v2
For bug reports, read
  http://openocd.org/doc/doxygen/bugs.html
embedded:startup.tcl:60: Error: Can't find openocd.cfg
in procedure 'script' 
at file "embedded:startup.tcl", line 60
Error: Debug Adapter has to be specified, see "interface" command
embedded:startup.tcl:60: Error: 
in procedure 'script' 
at file "embedded:startup.tcl", line 60
```

用strace 觀察輸出訊息如下：

```c
$ strace -f openocd 2>&1  | grep cfg
open("openocd.cfg", O_RDONLY)           = -1 ENOENT (No such file or directory)
open("/home/asdf/.openocd/openocd.cfg", O_RDONLY) = -1 ENOENT (No such file or directory)
open("/usr/local/share/openocd/site/openocd.cfg", O_RDONLY) = -1 ENOENT (No such file or directory)
open("/usr/local/share/openocd/scripts/openocd.cfg", O_RDONLY) = -1 ENOENT (No such file or directory)
write(2, "embedded:startup.tcl:60: Error: "..., 118embedded:startup.tcl:60: Error: Can't find openocd.cfg
```


從輸出訊息可以知道，openocd會依下面的順序讀取openocd.cfg

- 目前目錄的openocd.cfg
- ~/.openocd/openocd.cfg
- /usr/local/share/openocd/site/openocd.cfg
- /usr/local/share/openocd/scripts/openocd.cfg

所以接下來就是在openocd的原始碼中挑和你開發target可以使用config 檔案，放入~/.openocd、改成openocd.cfg。當然事情沒那麼簡單，解掉這個問題接下來還會有一些缺少檔案的問題，一樣靠strace就可以搞定。