# Stream I/O 和 EOF



# [你所不知道的C語言](http://hackfoldr.org/dykc/): Stream I/O 和 EOF
Copyright (**慣C**) 2018 [宅色夫](http://wiki.csie.ncku.edu.tw/User/jserv)

## 資料處理流程

![](https://i.imgur.com/tb1ak2D.png)
data flow for the standard input (0), output (1), and error (2) streams
[ [source](http://www.rozmichelle.com/pipes-forks-dups/) ]


在 UNIX 的設計哲學是 "Everything is a file"，扣除少數的例外 (如 BSD socket)，任何資源都可當作檔案來操作，不過當我們查閱 C 語言手冊和文獻時，"file" 的操作被分類在 stream I/O 中，聽起來有點奇怪吧？而 stream I/O 是 C 語言心裡最軟的一塊，這話怎説？

我們先來看 `<`[ctype.h](http://www.cplusplus.com/reference/cctype/)`>` 標頭檔宣告的 [islower()](http://www.cplusplus.com/reference/cctype/islower/) 函式，其作用很單純，就是判定輸入的「字元」是否為英文小寫字元，不過在宣告中，[islower()](http://www.cplusplus.com/reference/cctype/islower/) 函式的參數卻是 int 型態，類似的現象也出現於 getchar() 函式的回傳值，也是 int 型態。在現行 64 位元處理器架構上運作的作業系統常用 32-bit 表示 int 型態，甚至某些作業系統用 64-bit 表示 int 型態，讓我們不禁納悶：「C 語言不是很講究效率嗎？能用 1 個 byte (char) 處理的資料，為何要用 4 個 bytes 甚至更多 (int) 去表達呢？」

簡單來說，要考慮到 EOF (end-of-file)！後者跟編譯器與執行環境有關，上述的 islower() 或 getchar() 之所以將原本 char 型態可保存的資料「擴充」為 int 型態，即是考慮到輸入和輸出的過程可能被某種情況或條件給「打斷」，倘若要深入理解更多， 就得回到 stream I/O


## 有沒有 C++ 標準幫 Apple 背書的八卦

C++ 標準函式庫竟然有 [ios::good](http://www.cplusplus.com/reference/ios/ios/good/)
![](https://i.imgur.com/erYYXIq.png)
「iOS 真棒！」


## EOF 的發生往往不是單一程式的事

C99 規格書 7.19.6.2 談及 `fscanf` 一類函式的行為, 第 5 段:  

> "A directive composed of white-space character(s) is executed by reading input up to the first non-white-space character (which remains unread), or until no more characters can be read."

end-of-file 的設定是由輸入函式在讀取資料的過程中設定的。 

搜尋 [glibc 原始程式碼](https://sourceware.org/git/?p=glibc.git)，可發現 `EOF` 定義在 `libio/stdio.h`，但這是 `Define ISO C stdio on top of C++ iostreams`

```clike
/* The value returned by fgetc and similar
   functions to indicate the end of the file.  */
#define EOF (-1)
```

`ctype.h` 實作於 glibc 原始程式碼的 `ctype/ctype.h`

這裡要補充一點, 在有些系統 (特別是 UNIX 系統) 上, 每一行資料 (這裡只指 text 資料) 必須以 end-of-line 來分割, 包括最後一行. 否則, 某些程式語言在, 或系統處理最後一行時有可能會出問題。

〔資料〕<EOL>  
．．．  
〔資料〕<EOF> <-- 有可能會造成問題
  
最好是這樣:  
〔資料〕<EOL>  
．．．  
〔資料〕<EOL>  
<EOF>

以 ctrl-Z 做文字檔的 end-of-file marker 源自 DEC 早期的系統, 後來被 CP/M 借用, 再後來用在 MS-DOS 上。 
  
使用 ctrl-Z 的原因是因為在早期的檔案系統，檔案長度是以 128-byte 的 sector 為單位的, 當檔案的大小不是 sector 的整數倍數的話, ctrl-Z 用來標示檔案的真正結尾, 並以 ctrl-Z 把剩餘的 sector 填滿。

自 MS-DOS 2.0 起就可以正確的記綠檔案的正確長度, 已可不需要用 ctrl-Z 的機制了. 但這個機制到目前還在支援. 因為在某個情況下它非常有用。
  
當你在 console 上用鍵盤來輸入資料時, 可以用 ctrl-Z 來產生 end-of-file 訊息。

〔執行〕
1 2 3 4
5 6 7 8^Z
you entered 8 numbers.
  
UNIX 系統上用 ctrl-D 來 signal end-of-file.
  
以目前的系統來說, EOF 只是一個概念上的存在, 文字檔並不需要這個 marker，text editor 也不會自動的加入這個 marker.   

在 MS-Windows  上, ctrl-Z 不再做為檔案大小的依據, 也不會導致 file truncation. Ctrl-Z 可以存在文字檔內, 但它會影響輸入函式的行為: 如果檔案是以 text mode 來開啟的話, ctrl-Z 會終結輸入，即使 ctrl-Z 後面還有資料也是一樣。

[Standard I/O](https://blog.kuoe0.tw/posts/2013/02/22/acm-icpc-about-io/)
C++ iostream 的 `cin`, `cout` 和 C stdio 的 `scanf`, `printf` 效能比較
