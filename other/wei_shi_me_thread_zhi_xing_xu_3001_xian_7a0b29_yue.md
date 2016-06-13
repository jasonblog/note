# 為什麼 thread (執行緒、線程)越少越好?


個人至少 15 年前就有 multi-threading programming 的經驗，當時的畢業專題，就是在 Linux 上用 PThread + BSD socket 寫作遊戲地圖伺服器。

在 OS 課程中，站長還特別跑了中央圖書館一趟，影印了一堆冼鏡光老師曾發表於微電腦傳真上，各種跟 Concurrent Programming 有關的作品。

不過這幾年站長變得越來越反對使用 multi-threading(不是 multi-process 喔)，至少是盡量少用，為什麼呢？


##除錯困難


沒錯，難度會隨著 thread 的增加而變得越來越困難，入門的 IDE 的單步執行幾乎沒什麼用處，你得隨時注意 thread id 的變化，外加用上 debugger 一些進階功能。

這時候你會發現簡單如 printf 反而比較有用，不過新的問題又誕生了，你的除錯輸出函式有沒有 thread-safe？

講到 thread-safe，你又必須注意，你所呼叫的每個 API、library、system call...，有沒有 thread-safe？Compiler option 有沒有記得 enable thread-safe？

很多常用的 library 為了效率、平台考量，並沒有支援 thread-safe，而是把這項工作留給了 user(例如 C++ STL)，你必須替這些 library 加一層醜陋的 mutex、semaphore 來保證內部的操作滿足 atomic。

為了增加效率，你可能得再用上 reader/write lock、condition variable......哇

也許有人會說，multi-process 一樣要用上這些東西啊！問題是，process 之間要分享資訊必須要透過 IPC API，如果沒有透過 IPC，process 彼此之間很難影響對方，而且經由強迫使用 IPC，程式員也比較容易感知道自己開放了些什麼。而 multithread 一開始就把 thread 彼此的資訊毫無隱私的透露給對方，假如這些 thread 由一群良莠不齊的程式員一起開發，只要有一個不守規矩，含有這一群 threads 的 process 想不崩潰也難！

大神 Brian W. Kernighan and P. J. Plauger 在  `The Elements of Programming Style `說過：「Debugging is twice as hard as writing the code in the first place. Therefore, if you write the code as cleverly as possible, you are, by definition, not smart enough to debug it.」

(除錯的難度兩倍於一開始寫的代碼。因此，如果你過份賣弄技巧，根據定義，你將不夠聰明去除錯他。)

如果你在設計階段就把自己搞到油盡燈枯，只能說祝你好運！

##效能不如預期


如果是 Windows 環境，至少，微軟工程師都親口承認過。在 Network Programming for Microsoft Windows 2nd 這本由微軟工程師寫的書中，印證了無論是網路的吞吐量與造成的 CPU loading，multi-threads + blocking socket 的測試結果是最差的：





微軟給用戶的建議是，開啟的 thread 數量最好是：CPU x 2 + 1(沒記錯的話)

為什麼呢？因為如果你每個 connection 配給一個 thread，CPU 需要花費大量的時間於 context switch 上，另外如果不另外調整，每個 thread 預設就是會佔用 1MB 左右記憶體，如果 1000 個 connection 那就快 1G，您覺得這樣划算嗎？

然後微軟建議你用更難用難懂的 IOCP(I/O Completion Port)來寫程式，因為微軟自家的 WaitForMultipleObjects() 只能等待 64 組 events，select() 又是半殘只能用於 winsock(雖然說在 Linux 上這東西也不怎樣)。

直到最近，微軟終於聽從民意推出了 WSAPoll() 這個 API，不過很可惜的是，知名網路工具 curl(號稱被下載百萬次，幾乎所有的平台都有移植版本)的作者表示這個 API 有 bug，而且微軟還不怎麼願意修...= =

(我個人的感想是，微軟壓根就不想讓您用 winsock 搞他的 OS)

另外，使用 mutex、semaphore 必須在 kernel/user mode 間切換，這也會增加額外的效率損失。

##thread 與 OS 原本設計衝突


Linux 粉不用特別開心，乍看 Linux thread 比 win32 thread 出色，但實務上仍然有陷阱。

在 Linux 界有 Linux 版 「Advanced Programming in the UNIX® Environment」美譽；The Linux Programming Interface 一書的作者 Michael Kerrisk 表示：thread 會使得 signal 的處理變得更加複雜。

實際上也是如此，因為 signal 本來就已經是非同步的，往往就要加上一些程式碼避免 race condition，例如 mask signal，或是處理 EINTR 錯誤，這時候再加上一堆 threads，差不多就是等於先把自己放血再游過有一群鯊魚的海洋。

signal 部份 UNIX 人士覺得是歷史遺跡，最好不要用。但是 Linux System Programming(作者Robert Love是知名 kernel hacker)，本書可以說是前面那本的精簡版，仍然為 signal 保留了不少章節，證明 signal 是很難避免的，但 thread 卻不一定非用不可...

##multi-threading 不再是主流


隨便舉一堆例子可以發現，目前的主流程式庫、網路伺服器等軟件幾乎都不再使用 multi-threading，而是往非同步、事件驅動靠攏。以 Linux 環境來說無一例外都是使用 epoll 達成，例如以下軟件：


- node.js
- Python Twisted
- Ruby EventMachine
- Apache Mina
- Memcached
- Chromium
- Jboss Netty
- lighttpd
- Nginx
- boost::asio
- ZeroMQ

上述軟件，要不就是 single thread(如 node.js)，要不就是僅使用少量的 threads(如同前述的 CPUx2+1 公式)

為什麼呢？因為大部份的人已經體驗到，寧可在設計階段付出代價(雖然說  Reactor、Proactor 這些 pattern 沒有那麼直覺)，也不要在除錯階段付出更高的代價，而且得到的效率不如付出的成本。

有沒有想起 C++？一開始的願景也是給大家一個強大、無所不包的程式語言。結果我們得到了什麼？超慢的編譯速度、有如天書般的 template、各種看不懂的錯誤訊息(尤其是編譯 template 產生的)，更好笑的是 Google 還禁止其團隊使用 C++ 的例外處理(個人認為是因為 C++ 缺少垃圾收集機制)...如果不是靠早年 AT&T 強力行銷，C++ 早掉出程式語言排行前 10 名

最後我們看看電腦科學界一代巨匠 Donald Knuth 對 multithreading 的評論：

[「如果多線程的想法被證明是失敗的，我一點都不會感到驚訝」](http://www.eygle.com/archives/2008/07/donald_knuth.html)