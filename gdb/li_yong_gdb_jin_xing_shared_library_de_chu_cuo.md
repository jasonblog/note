# 利用gdb進行shared library的除錯


寫程式時, 遇到bug最常使用的除錯工具就是gdb了. gdb會利用ld.so這個dynamic linker/loader, 自動將我們的程式載入記憶體, 並且在執行過程中將程式所使用到的shared library載入. 然而, 如果我們要debug的是已經compile成shared library的function時, 這種on-demand loading的運作方式則會造成我們的不便, 因為我們無法預先設定breakpoint以便在程式呼叫到shared library中的function時, 將程式執行暫停.

因此, 如果要以gdb對已經compile成shared library的function進行除錯時, 可以採用下列方式來進行在shared library中的function的除錯工作:

- 首先, 當然是在編譯shared library時, 必須有debug information, 也就是compile時要加上-g的option

- 再來就是告訴ld.so, 在載入程式時,一併載入我們所要debug的shared library, 只要利用LD_PRELOAD環境變數即可. LD_PRELOAD是告訴ld.so, 在程式一開始載入時, 就順便一起載入LD_PRELOAD環境變數所指定的shared library.
export LD_PRELOAD=要debug的shared library路徑

- 執行gdb進行程式除錯, 並且先隨便設定一個中斷點
break main

- 輸入run讓gdb開始執行程式, 此時gdb應該會一下子就暫停程式的執行. 這時候就可以確定一下我們要debug的shared library是否已經載入

```sh
info share
```
此時應該就可以找到我們在shell中利用LD_PRELOAD環境變數要求ld.so載入的shared library了. 接下來就可以設定shared library中的function為中斷點並且進行debug了.

