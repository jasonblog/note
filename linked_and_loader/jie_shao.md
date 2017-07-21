# 介紹


電腦架構 : CPU / Memory / IO 設備

目前 CPU 最高頻率為 4GHz，已經達到實體限制

API = Application Programming Interface

Linux API : Glibs -> POSIX

Window API : Win32

## 磁碟讀取

8000 byte file

0 - 4096 byte -> 1000 - 1007 sector (every sector 512 byte ; 8*512 = 4096 byte )

4097 - 8000 byte -> 2000 - 2007 sector (every sector 512 byte ; 8*512 = 4096 byte )

ext3 -> 1000 - 1007 -> 2000 - 2007 用鏈結串列當作檔案儲存裝置

IDE0 的 I/O = 0x1F0 - 0X1F7 & 0x376 - 0x377

如果要讀取 1000 的 LBA 開始的 8 個 sector

```sh
0x1F3 - 0X1F6 的 4 byte 為 LBA 位址
LBA 1000 位址 = 0x3E8
0x1F3 = 0x00 (1 Byte)
0x1F4 = 0x00 (1 Byte)
0x1F5 = 0x03 (1 Byte)
0x1F6 = 0xE8 (1 Byte)

0x1F2 為要讀取幾個 sector，要讀取 8 個所以寫 8
0x1F7 為操作的動作，讀取動作是 0x20


完整指令是
out 0x1F3, 0x00
out 0x1F4, 0x00
out 0x1F5, 0x03
out 0x1F6, 0xE8
out 0x1F2, 0x08
out 0x1F7, 0x20
```

## 記憶體位址

virtual address 解決了位址隔離問題

位址隔離就是說，不同 process 有各自的記憶體空間

也就是 process 會有自己的 virtual address 範圍

HW 會去判斷不同 process 是否存取到自己的區段

如果存取超出範圍就拒絕存取

所以 virtual address 可以解決個別 process 隔離問題

## 分頁

virtual address 不能解決 ram 使用率問題

因為傳統上程式需要整個搬到 ram 執行

而 Page 分頁的作法就是為了解決 ram 的使用效率

概念是程式在執行時，並不是每一段都需要用到

那只要將需要用到的部分放入 ram 即可，其他程式區段放在硬碟中

Paging 將位址空間分成固定大小的頁面

```sh
1.頁面大小由硬體決定
2.如果硬體可以支援不同大小的分頁，可由 OS 決定分頁大小
```

例如 Pentium CPU 支援 4KB 或 4MB 的分頁大小，目前都使用 4KB 為主

假設 PC 是 32位元，可以定址範圍是 4GB，分頁為 4KB，總共就有 104_8576 頁

所以 CPU 會送出 virtual address 給 MMU，MMU 會轉換為 phycial address

MMU 通常整合在 CPU 內部了

##Thread

Process 執行有 3 個區塊 global、stack、heap


```sh
Global:
- 全域變數
- static的靜態變數

Stack: (可預測性且後進先出的模式)
- local variable
- function/method parameter
- function/method return address
-> stack overflow : 過多的函式呼叫(例如：遞迴太深)、或區域變數使用太多


Heap: (不可預測動態產生)
- 如 Jave 的 new 
- 如 C 的 malloc/realloc/free
-> heap overflow : 過多的 new 物件
```

Process 由一個或多個 Thread 組成

Thread 有各自的 ID、PC、register、stack

但是彼此 Thread 共用 Process 的程式碼、資料區段(全域變數)、空間、資源(開檔、信號)

Thread 好處在於需要等待的狀態

例如一個 T 處理網路下載，但需要等待，這時另一個 T 還是可以處理使用者互動

不會因為要等網路而當掉

## Thread 存取權限

T 可以存取 process 中在記憶體的所有資料，以及其他 T 的 stack

T 自己擁有:

```sh
stack (雖然還是可以存取其他 T 的 stack，但一般來說不會)
Thread local storages(TLS)
Register、PC
```


