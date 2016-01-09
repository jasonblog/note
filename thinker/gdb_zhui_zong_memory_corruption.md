# GDB 追蹤 memory corruption


在不久之前，在 GDB 自動化 debug 這篇文章中，簡單提到如何用 script 控制 gdb 。而今天又因為 MadButterfly 的範例程式裡的某個 bug ，再次使用相同的技巧。這次遇到的是一種典型的錯誤，`某塊記憶體不知何時被修改，造成程式錯誤。`
我將問題簡化一下。有一個變數 V，在程式一啟動之後就會 initialize 成特定值 k。但因未知原因，結果 V 不知何時被改成 m。為了要查出倒底是誰改了 V 的值，一般的做法是檢視程式流程，找出所有可能會改到 V 的地方。但有時侯根本是 pointer 亂指，不小心改到該變數。這時肉眼檢視的方法就不太可行了。這時，最重要的是找出程式是執行到什麼步驟時，該變數會被改變。

如果以一般的做法，就是到處 printf() ，然後不斷的 compile 、重新執行。這很沒有效率，因為無法動態檢視其它變數，以進行比較。比較有效的方法是，在該變數被修改時能自動讓 gdb 停下來。這時，在 gdb 設定 watchpoint 就可以達到這個目的。但，有些平臺的 hardware 並不支援 watchpoint ，這時 gdb 會以連續 single step 的方式執行，以達到 watchpoint 的效果。但這會非常非常的慢。如果程式複雜的話，這幾乎不可行。這時，透過適當的設定 break point ，更能快速的鎖定問題的發生點。

做法是，先把程式執行流程中，可疑的地點都設定 break point 如下
```c
break xxx.c:123 if V != k
```

這時，這個 break 會在 xxx.c:123 的位置，檢查變數 V 是否為 k。若不為 k ，則程式會暫停執行，否則 break point 不會發生效果。這時，你可以到處設定 break point ，將問題鎖定在一個範圍，並不斷的縮小範圍，直到找到特定的位置。這方式的好處是你可以快速的設定，然後再重新執行。並且可檢視 process 的狀態，以決定新的 break point 要加在哪。

有時侯，你要監看的記憶體是動態配置的。因此，你必需在配置時才知道其位址。這時，可在配置該記憶體的指令設定 break point ，並記下配置的位址。
```c
break xxx.c:567
commands
silent
set $addr=p
continue
end
```
假定，在 xxx.c:567 的前一個指令，會將配置到的記憶體位址存在 local variable p。那麼上面的設定可以將該位址存在 gdb 的 convenience variable $addr 裡。那麼我們就可以在其它 break point 裡檢查該位址的內容。例如

break xxx.c:123 if *$addr != k
就能在程式執行到 xxx.c:123 位置時，檢查 $addr 所指的記憶體位址內容。

這些設定，可以寫在一個文字檔，例如 foo.txt
```c
break xxx.c:567
commands
.......
end
```
那麼我們就可以透過 gdb 參數，不斷重複這些設定

```c
gdb -x foo.txt your_progm
```