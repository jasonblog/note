# 開源網絡庫的分析libevent muduo nginx


每一個開源項目存在都有它的道理和意義，不同的思想有不同的優缺點。

- libevent：這是一個用純C寫的開源庫，屬於一個輕量級的網絡中間件。其中用到的基本數據結構也是非常巧妙。展現反應堆模型的基本使用方法。不同的事件對應不容的處理方法。I/O 定時 信號。三種事件的處理單獨分開，又通過事件驅動融合在一起。
專欄地址：http://blog.csdn.net/column/details/libevent.html

- muduo:這是一個用純c++寫的庫，僅僅在linux下使用，one loop per thread的思想貫穿其中，將I/O 定時 信號都通過文件描述符的方式融合在一起，三類事件等同於一類事件來看待。這也是由於linux操作系統的支持。並沒有使用libevent中提到的自己寫的數據結構。同樣很好
專欄地址：http://blog.csdn.net/column/details/muduo.html

- nginx：這是一個高性能服務器使用，不同於上述兩種，多進程的使用，代碼量很大，這個需要大量的時間細細品味！！！！  