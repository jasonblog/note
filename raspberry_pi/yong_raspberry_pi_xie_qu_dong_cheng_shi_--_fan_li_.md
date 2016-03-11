# 用 Raspberry pi 寫驅動程式 -- 範例1：LED

寫驅動程式的時候，我們要先定義他的規格(specification)：

- 透過3個gpio（General Purpose I/O）去控制3個LED，gpio的位置在載入模組(module)的時候(modprobe, insmod)可以額外設定而不需要重新編譯模組。
- 控制方法：寫入/dev/LED_n (n=0,1,2)，若是寫入1則啟動LED，寫入0則關閉。
- 每個連入系統的使用者都可以控制

接起來就像這樣：


DSC_0114.JPG