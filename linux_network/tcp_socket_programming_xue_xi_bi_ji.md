# TCP Socket Programming 學習筆記




有別於 IPC ，Socket 是用於網路上不同程序的互相溝通，比如說流覽器要怎麼跟 Web Server 拿取資料、Messenger 訊息的收發、 ftp 檔案的上傳與下載等等，在現今的網路編程中，Socket 可以說是無所不在。

至今 Socket 也應不同的需求或 OS 衍生出了不少版本，這篇筆計主要是討論 Linux 的 socket ，並專注在實現 TCP 編程：