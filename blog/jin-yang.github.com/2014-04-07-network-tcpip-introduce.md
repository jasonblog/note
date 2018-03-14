---
title: TCP/IP 協議簡介
layout: post
comments: true
language: chinese
category: [linux, network]
keywords: linux,tcp,ip,協議,面向鏈接
description: TCP 協議是 "面向連接" 的，整個過程被分為幾個階段：準備、協商、建立連接、管理以及最後的連接終止，同時可以管理多個鏈接。接下來，我們看看 TCP 協議是如何實現的。
---

TCP 協議是 "面向連接" 的，整個過程被分為幾個階段：準備、協商、建立連接、管理以及最後的連接終止，同時可以管理多個鏈接。

接下來，我們看看 TCP 協議是如何實現的。

<!-- more -->

## 簡介

如上，TCP 可以同時管理多個連接，因此需要賦予每個連接一個唯一的標識進行識別，這個標識就是連接兩端的 IP 以及端口號 (共4個key)，從而保證了連接後續的建立、管理和中斷都是獨立分開的。

對於各個狀態遷移採用 FSM (Finite State Machine，有限狀態機) 進行描述，其包含了如下的幾種狀態：

* state：在一個給定的時間，協議應用（protocol software）的環境以及狀態的詳細說明；
* transition：從一個狀態到另一個狀態的動作；
* event：引起狀態間切換的事件；
* action：設備在切換到另一個狀態之前，所做的對於 event 的迴應。

下圖是一個 TCP 有限狀態機的簡化版本。

![TCP/IP Finite State Machine FSM]({{ site.url }}/images/network/tcpip_The-TCP-Finite-State-MachineFSM.png "TCP/IP Finite State Machine FSM"){: .pull-center}

### TCB

由於每個連接都是獨立的，所以我們使用了一個定製的數據結構來將與連接相關的信息分別存儲，這個就是 TCB (Transmission Control Block，傳輸控制模塊)。

在連接啟動建立之前，內核會做一些準備工作，其中之一就是建立起維護連接信息的 TCB ，而且兩端的 TCB 都是由兩端自己的設備去維護。

TCB 保存了與連接相關的所有的重要信息，比如兩端端口號，用於連接識別；用於管理連接數據流的 buffer 的內存指針；TCB 也用於窗口切換機制，用來保存所接收到的數據中有哪些已經被確認和尚未被確認的變量，當前窗口大小等等。

### 報文頭部

按照 OSI 的七層模型，ARP 工作在第二層 (Data Link，Frame)，IP 在第三層 (Network, Packet)，第四層 (Transport, Segment)。數據依次打包，傳到對端後，各個層解析自己的協議，然後把數據交給更高層的協議處理。

![TCP/IP Protocal Header]({{ site.url }}/images/network/tcpip_TCP-Header.png "TCP/IP Protocal Header"){: .pull-center}


<!--
1. 源端口號：數據發起者的端口號，16bit
2. 目的端口號：數據接收者的端口號，16bit
3. 序號：32bit的序列號，由發送方使用
4. 確認序號：32bit的確認號，是接收數據方期望收到發送方的下一個報文段的序號，因此確認序號應當是上次已成功收到數據字節序號加1。
5. 首部長度：首部中32bit字的數目，可表示15*32bit=60字節的首部。一般首部長度為20字節。
6. 保留：6bit, 均為0
7. 緊急URG：當URG=1時，表示報文段中有緊急數據，應儘快傳送。
8. 確認比特ACK：ACK = 1時代表這是一個確認的TCP包，取值0則不是確認包。
9. 推送比特PSH：當發送端PSH=1時，接收端儘快的交付給應用進程。
10. 復位比特（RST）：當RST=1時，表明TCP連接中出現嚴重差錯，必須釋放連接，再重新建立連接。
11. 同步比特SYN：在建立連接是用來同步序號。SYN=1， ACK=0表示一個連接請求報文段。SYN=1，ACK=1表示同意建立連接。
12. 終止比特FIN：FIN=1時，表明此報文段的發送端的數據已經發送完畢，並要求釋放傳輸連接。
13. 窗口：用來控制對方發送的數據量，通知發放已確定的發送窗口上限。
14. 檢驗和：該字段檢驗的範圍包括首部和數據這兩部分。由發端計算和存儲，並由收端進行驗證。
15. 緊急指針：緊急指針在URG=1時才有效，它指出本報文段中的緊急數據的字節數。
16. 選項：長度可變，最長可達40字節
-->

上圖是 TCP 頭部，來自 [nmap tcpip-ref](http://nmap.org/book/tcpip-ref.html)，需要注意的是 A) TCP 包是沒有 IP 地址，那是 IP 層的事，但是有源端口和目標端口。

上圖中比較重要的是，A) Sequence Number 是包的序號，用來解決網絡包亂序問題；B) Acknowledgement Number 用於確認收到包，用來解決丟包問題；C) Window 又叫 Advertised-Window 或者 Sliding Window 滑動窗口，用於解決流控；D) TCP Flag ，也就是包的類型，主要是用於操控 TCP 的狀態機的。

## 三次握手，建立連接

三次握手主要用於建立連接，主要過程如下所示。

![TCP/IP Three Way Handshaking]({{ site.url }}/images/network/tcpip_Three-Way-Handshaking.png "TCP/IP Three Way Handshaking"){: .pull-center}

簡單看下連接的建立過程，在建立鏈接之前，server 已經處於 LISTEN 狀態，而客戶端處於 CLOSED 狀態。

1. 第一次握手，客戶端設置 TCB 併發送 SYN(syn=j) 包到服務器，嘗試建立連接，而客戶端進入 SYN-SENT 狀態，等待服務器確認。
2. 第二次握手，服務器收到 SYN 包，必須確認客戶的 SYN(ack=j+1)，同時自己也發送一個 SYN (syn=k) 包，也即 SYN+ACK 包，此時服務器進入 SYN_RECEIVED 狀態，同時將該鏈接保存在半連接隊列。
3. 第三次握手，客戶端收到服務器的 SYN+ACK 包，向服務器發送確認包 ACK(ack=k+1)，客戶端進入 ESTABLISHED 狀態；當服務器接收到 ACK 後也進入 ESTABLISHED 狀態，完成三次握手。

注意，對於服務端會將 ESTABLISHED 狀態的連接移入 accept 隊列，等待應用調用 accept() 。

可以看到，在服務端，建立連接涉及兩個隊列：

* 半連接隊列，保存了處於 SYN_RECV 狀態的連接，表示服務器已經收到 SYN 包，並向客戶發出確認，在等待客戶端的確認。隊列長度由 net.ipv4.tcp_max_syn_backlog 設置，一般默認值為 512 或者 1024，超過這個數量，系統將不再接受新的 TCP 連接請求。
* 連接隊列，保存 ESTABLISHED 狀態的連接。隊列長度為 min(net.core.somaxconn, backlog)，其中 backlog 是調用 listen(int sockfd, int backlog) 時設置。

### 序列號同步

通過 TCP 的三次握手，除了建立鏈接之外，設備上的 TCP 層也進行一些必須的信息交換，包括了 A) 第一次數據傳輸時使用的序列號，B) 有關如何操作該連接的一些參數。這一過程一般被稱為 “序列號同步(Sequence Number Synchronization)”，這是建立連接過程中一個很重要的部分。

![TCP/IP Server Client Process]({{ site.url }}/images/network/tcpip_server-client-process.jpg "TCP/IP Server Client Process"){: .pull-center  width="50%"}

在建立鏈接時，雙方需要相互通知自己的初始序列號 (Inital Sequence Number, ISN)，也就是 Synchronize Sequence Numbers。如上圖中的 x 和 y，該值會作為以後的數據通信的序號，以保證應用層接收到的數據不會因為網絡上的傳輸的問題而亂序，TCP 也就是用這個序號來拼接數據。

可以通過如下圖的方式查看序列號是如何變化的，也就是使用 Wireshark 菜單中的 Statistics => Flow Graph… 。

![TCP/IP Sequence Number Process]({{ site.url }}/images/network/tcpip_sequence-number-process.jpg "TCP/IP Sequence Number Process"){: .pull-center }

你可以看到，序列號的增加是和傳輸的字節數相關的。上圖中，三次握手後，來了兩個長度為 1440 的包，而第二個包的 SeqNum 就成了1441，然後第一個 ACK 回的是 1441，表示第一個 1440 收到了。

注意：Wireshark 抓包程序看3次握手，會發現SeqNum總是為0，這是為了顯示更友好，使用了相對序號，只要在右鍵菜單中的 protocol preference 中取消掉就可以看到 “Absolute SeqNum” 了。

更加詳細的可以參考 [查看 TCP/IP 三次握手](/blog/network-wireshark.html)，或者參考 [Understanding TCP Sequence and Acknowledgment Numbers](http://packetlife.net/blog/2010/jun/7/understanding-tcp-sequence-acknowledgment-numbers/)
，或者 [本地文檔](/reference/linux/network/Understanding TCP Sequence and Acknowledgment Numbers.mht) 。

### 序列號相關

有一個問題，我們為什麼不能每次都把數據的第一位的序號編為 1，或者一個固定值 ? 如果採用固定的序列號，可能會導致不同鏈接的數據流混亂。

假設我們在 A 和 B 之間建立起了一個連接， A 向 B 發送了一個 byte1 到 byte30 的消息，但是此時網絡出現問題，這個數據流被延遲了，最後，這個連接中斷。此時我們重啟了 A 到 B 的連接， A 向 B 發送了一個以 1 為序列開頭的數據流，然而此時可能之前的 byte1 到 byte30 發送到了 B ， B 也會認為這 30 個 byte 可能就是來自於後來新連接從而接受，那麼此時就出現了錯誤。

為此，在 RFC793 中規定，ISN 會使用一個與時間綁定的計數器，在 TCP 層啟動的時候初始化，每 4 微秒自增 1，直到溢出歸 0，這樣，一個 ISN 週期大約是 4.55 小時。

因為，我們假設我們的 TCP Segment 在網絡上的存活時間不會超過 Maximum Segment Lifetime，只要 MSL 的值小於 4.55 小時，那麼，我們就不會重用到 ISN。

這種解決方式會帶來另外一個問題，就是 ISN 變得可以被預測：可以通過分析連接的之前的 ISN 然後預測後來的 ISN ，這就造成了一個安全隱患，這個漏洞在過去曾經被利用發起國著名的 Mitnick 攻擊。為了解決這個問題，現在使用的隨機數代理的自增。

<!--

<h3>TCP序列號的協商</h3>
<p>設備選好自己使用的 ISN 以後，就會把 ISN 的值填在 SYN 消息的序列號區中，併發送給對端，對端接收到以後，會迴應一個 ACK （也可能包含自己的 SYN ），在 ACK 消息中，在“確認編號”區會設一個值，這個值等於接收到的 SYN 的 ISN 的值加一。這代表著設備接收到的下一個序列號來自於自己的對端。ISN 實際代表著最後收到的 byte 的序列號（假設連接是新建立並且上沒有數據傳輸的情況下），具體這兩個域是如何控制數據傳輸的，會在後面做介紹。<br><br>

關鍵點：作為連接建立就緒過程中的一部分， TCP 連接的兩端都會告知對方自己在做第一次傳輸的時候將是用什麼序列號，告知方式是將這個值放在 SYN 消息頭部的序列號區域。對端收到以後，會對其進行確認，確認方式就是在 ACK 消息的頭部的“確認編號”區域填入一個增加 1 後的數值。這個過程就叫做通訊序列號的協商。<br><br>

這裡有一個簡單的例子描述三次握手過程中的序列號是如何工作的，我選用了較小 ISN 方便描述，但是要記住 ISN 可以是一任意一個 32 位二進制的值。

<ol type="A"><li>
Client發出連接請求：Client 選在一個 ISN 給其傳輸使用，假設為 4567 。那麼 Client 會發送一個序列號域值為 4567 的 SYN 消息。</li><br><li>

Server 進行確認併發出連接請求： Server 使用 12998 作為其連接請求的 ISN 。接收到 Client 的 SYN 以後，它會發送一個 SYN+ACK ，其中 ACK 的確認編號區的值為 4568（自增 1 ）， SYN 的序列號域值為 12998 。</li><br><li>

Client 進行確認：Client 返回個 Server 一個 ACK 消息， ACK 消息的確認編號區的值為 12999（Server 的 SYN 消息 ISN 自增 1 ）。</li></ol>
<center><img src="pictures/Three-Way-Handshaking-Sequence.png" height="410" width="450"></center><br>
現在連接就已經就緒了，準備好傳輸數據。 Client 發送數據的第一個 byte 的編號就會是 4568 ， Server 的第一個則會是 12999 。</p>
-->


### SYN 超時與 SYN Flood 攻擊

在建立鏈接時，如果服務端接到了客戶端發的 SYN 後回了 SYN-ACK 後客戶端掉線了，服務端沒有收到客戶端回來的 ACK，那麼這個連接處於一箇中間狀態，即沒成功，也沒失敗。於是，服務端對於在一定時間內沒收到 ACK 的請求會重發 SYN-ACK。

在 Linux 中，默認重試次數為 5 次，重試的間隔時間分別為 1s, 2s, 4s, 8s, 16s，在第 5 次發出後還要等 32s 都知道第 5 次也超時了，所以，總共需要 1s + 2s + 4s+ 8s+ 16s + 32s = 2^6 -1 = 63s，TCP 才會把斷開這個連接。

SYN Flood 攻擊就是給服務器發了一個 SYN 後，就下線了，於是服務器需要默認等 63s 才會斷開連接，這樣，攻擊者就可以把服務器的 SYN 連接的隊列耗盡，讓正常的連接請求不能處理。

對於正常請求，應該調整三個 TCP 參數：A)  tcp_synack_retries 減少重試次數；B) tcp_max_syn_backlog 增大SYN連接數；C) tcp_abort_on_overflow 處理不過來乾脆就直接拒絕連接了。


### TCP SYNC cookies

SYN Cookie 是對 TCP 服務器端的三次握手協議作一些修改，專門用來防範 SYN Flood 攻擊。

其原理是，在 TCP 服務器收到 TCP SYN 包並返回 TCP SYN+ACK 包時，不分配一個專門的數據區，而是根據這個 SYN 包計算出一個 cookie 值。在收到 TCP ACK 包時，TCP 服務器在根據那個 cookie 值檢查這個 TCP ACK 包的合法性。如果合法，再分配專門的數據區進行處理未來的 TCP 連接。

於是，Linux下給了一個叫tcp_syncookies的參數來應對這個事——當SYN隊列滿了後，TCP會通過源地址端口、目標地址端口和時間戳打造出一個特別的Sequence Number發回去（又叫cookie），如果是攻擊者則不會有響應，如果是正常連接，則會把這個 SYN Cookie發回來，然後服務端可以通過cookie建連接（即使你不在SYN隊列中）。請注意，請先千萬別用tcp_syncookies來處理正常的大負載的連接的情況。因為，synccookies是妥協版的TCP協議，並不嚴謹。

## 四次握手，關閉連接

關閉相對來說要複雜，這是由於兩端設備都在相互的發送/接收數據，當一端告訴對端 “我打算關閉這個連接” 時，而對端以及相應的應用程序可能完全不知道自己的對端正在打算關閉這個連接，所以為了能夠優雅的關閉當期的連接並且還要保證不會產生數據丟失，這個步驟就會相對的複雜一些。

通過信息同步，當兩端的程序都意識到 “這個連接就要被關閉” 並且已經停止發送數據的時候才能真正的開始著手關閉這個連接。而對於連接的一端來說，連接的關閉意味著自己停止發送數據，但是還可以接受從對端傳來的數據，直到對端也停止發送。

說是四次握手，而實際上是兩次，這是因為 TCP 是全雙工的，發送方和接收方都需要 FIN 和 ACK，只不過，有一方是被動的，所以看上去就成了所謂的四次揮手。如果兩邊同時斷連接，那就會就進入到 CLOSING 狀態，然後到達 TIME_WAIT 狀態。

### 標準連接關閉過程

兩端通過發送 FIN 消息來告知對端自己將關閉連接，當對端接收到 FIN 消息以後，會返回一個 ACK 消息告知 FIN 消息已經收到。只有當雙方都發送了 FIN 並且接收到了 ACK 才會被認為已經完全的斷開。

![TCP/IP Four Way Handshake]({{ site.url }}/images/network/tcpip_Four-Way-Handshake.png "TCP/IP Four Way Handshake"){: .pull-center}

### 同時關閉

如下圖所示，在這個過程中有很多是相對稱的地方，狀態對稱，動作和結果也對稱。在 CLOSED 狀態時，也就是沒有連接時， TCP 層會保證所有重要的數據已經發送到應用層，有時候可以理解成是一個 “push” 。 TCB 也會在連接關閉完成後被銷燬。

關鍵點：就像連接可以通過兩端同時主動 OPEN 建立，連接也可以通過兩端同時中斷來完成關閉。在這種情況下，轉換過程會有一些不同，兩端的設備都是先用 ACK 回覆對端發來的 FIN ，然後再等待接受返回的 ACK ，最後再經過 2 倍 MSL 來確保消息和數據已經傳輸完成，終於關閉了連接。

![TCP/IP Four Way Handshake Sync]({{ site.url }}/images/network/tcpip_Four-Way-Handshake-Sync.png "TCP/IP Four Way Handshake Sync"){: .pull-center}

### CLOSE-WAIT 和 TIME-WAIT

收到 FIN 消息的服務器可能會在 CLOSE-WAIT 狀態等待相當長的一段時間，直到對應的應用程序告訴它 “我已經準備好關閉連接了”。TCP 不能假設自己會需要等待多久，在這一段時間裡， Server 可能仍在發送數據，而 Client 則一直在接受，但是 Client 已經不會再發給 Server 數據了。

到最後，Server 終於發出了 FIN 消息要關閉自己這一端的連接了，那麼起初發送 FIN 消息的機器即 Client 會返回一個 ACK 進行確認。但是 Client 不能在發送完 ACK 立刻就進入 CLOSED 狀態，因為它必須要等待 ACK 確確實實的已經發送到了對端，正常情況下這個過程很快，但是還是要考慮不正常情況可能造成的延遲。

從 TIME_WAIT 到 CLOSED 有一個 2*MSL 的超時設置 (RFC793 定義為 2min，而 Linux 設置為 30s)，這麼作主要有兩個原因：

* 要確保足夠長的時間能讓 ACK 到達連接對端，並且還有可能會要求重傳，如果被動關閉的那方沒有收到 ACK，就會觸發被動端重發 FIN，一來一去正好 2 個 MSL。

<!--
這是因為：雖然雙方都同意關閉連接了，而且握手的 4 個報文也都協調和發送完畢，按理可以直接回到 CLOSED 狀態；但是因為我們必須要假想網絡是不可靠的，你無法保證你最後發送的 ACK 報文會一定被對方收到，因此對方處於 LAST_ACK 狀態下的 SOCKET 可能會因為超時未收到 ACK 報文，而重發 FIN 報文，所以這個 TIME_WAIT 狀態的作用就是用來重發可能丟失的 ACK 報文，並保證於此。<br><br>

如果這時被動方會重新發 FIN ，如果這時主動方處於 CLOSED 狀態，就會響應 RST 而不是 ACK，所以主動方要處於 TIME_WAIT 狀態，而不能是 CLOSED 。
-->

* 第二個原因就是需要為即將關閉的當前連接和後繼的新連接之間提供一個 "緩衝間隔"，避免不同鏈接數據造成混亂的可能。主要防止上一次連接中的包，迷路後重新出現，影響新連接 (經過2MSL，上一次連接中所有的重複包都會消失)。

TCP 標準規定 TIME-WAIT 時間是兩倍的 MSL，在現在的網絡中，這是一個常量，所以 TCP 層允許程序使用一個較小的值去代替，只要能保證工作正常即可。

MSL (最大分段生存期) 用以指明 TCP 報文在 Internet 上最長生存時間，每個具體的 TCP 實現都必須選擇一個確定的 MSL 值。RFC1122 建議是 2 分鐘，但 BSD 傳統實現採用了 30 秒。

可以參考 [TIME_WAIT and its design implications for protocols and scalable client server systems](http://www.serverframework.com/asynchronousevents/2011/01/time-wait-and-its-design-implications-for-protocols-and-scalable-servers.html)，或者 [本地文檔](/reference/linux/network/TIME_WAIT and its design implications for protocols and scalable client server systems - AsynchronousEvents.mht) 。

## 滑動窗口

TCP 需要保證數據傳輸的質量，會對發送的數據進行跟蹤，解決可靠傳輸以及包亂續的問題。這種數據管理主要有以下兩大關鍵功能：A) 可靠性，保證數據確實到達目的地，未到達，則能夠發現並重傳；B) 數據流控，管理數據的發送速率，以使接收設備不致於過載。

要完成這些任務，整個協議圍繞滑動窗口確認機制來進行。所以，TCP 必需要知道網絡實際的數據處理帶寬或是數據處理速度，這樣才不會引起網絡擁塞，導致丟包。所以，TCP 引入了一些技術來做網絡流控，滑動窗口就是其中之一。

TCP 頭有一個字段 Window，用於接收端告訴發送端自己還有多少緩衝區可以接收數據，於是發送端就可以根據這個接收端的處理能力來發送數據，而不會導致接收端處理不過來。

![TCP/IP Slide Window]({{ site.url }}/images/network/tcpip-slide-window-struct.jpg "TCP/IP Slide Window"){: .pull-center width="70%" }

上圖是發送和接收端的相關數據結構，可以看到：

* 接收端 LastByteRead 指向了 TCP 緩衝區中讀到的位置，NextByteExpected 指向的是收到的連續包的最後一個位置，LastByteRcved 指向的是收到的包的最後一個位置，我們可以看到中間有些數據還沒有到達，所以有數據空白區。
* 發送端的 LastByteAcked 指向了被接收端 Ack 過的位置（表示成功發送確認），LastByteSent 表示發出去了，但還沒有收到成功確認的 Ack，LastByteWritten 指向的是上層應用正在寫的地方。

於是，接收端在給發送端回 ACK 中會彙報自己的 `AdvertisedWindow = MaxRcvBuffer - LastByteRcvd - 1`；而發送方會根據這個窗口來控制發送數據的大小，以保證接收方可以處理。

![TCP/IP Slide Window]({{ site.url }}/images/network/tcpip-slide-window-send.png "TCP/IP Slide Window"){: .pull-center width="90%" }

上圖中的黑框就是滑動窗口，整個緩衝區被分成了四個部分：#1 已收到 ACK 確認的數據；#2 已經發送但還沒收到 ACK；#3 在窗口但還沒有發出，接收方還有空間；#4 窗口以外的數據，接收方沒空間。

### Zero Window

![TCP/IP Zero Window]({{ site.url }}/images/network/tcpip-tcpswflow.png "TCP/IP Zero Window"){: .pull-center width="70%" }

上圖是一個處理緩慢的 Server 把 Client 的 TCP Sliding Window 給降成 0 ，此時 Client 將不再發送數據，如果接收方一會兒 Window Size 可用了，怎麼通知發送端呢？

TCP 使用 Zero Window Probe, ZWP 技術，也就是說，發送端在窗口變成 0 後，會發 ZWP 包讓接收方響應它的 Window 大小，一般會重試 3 次，第次間隔約 30-60s，如果 3 次後仍為 0 ，有的協議實現就會發 RST 把鏈接斷了。

<!--
注意：只要有等待的地方都可能出現DDoS攻擊，Zero Window也不例外，一些攻擊者會在和HTTP建好鏈發完GET請求後，就把Window設置為0，然後服務端就只能等待進行ZWP，於是攻擊者會併發大量的這樣的請求，把服務器端的資源耗盡。（關於這方面的攻擊，大家可以移步看一下Wikipedia的SockStress詞條）
另外，Wireshark中，你可以使用tcp.analysis.zero_window來過濾包，然後使用右鍵菜單裡的follow TCP stream，你可以看到ZeroWindowProbe及ZeroWindowProbeAck的包。
-->

### Silly Window Syndrome

如果接收方太忙了，來不及取走 Receive Windows 裡的數據，那麼，就會導致發送方越來越小；到最後，如果接收方只能騰出幾個字節並告訴發送方現在有幾個字節的 window，而發送方會義無反顧地發送這幾個字節。

<!--
要知道，我們的TCP+IP頭有40個字節，為了幾個字節，要達上這麼大的開銷，這太不經濟了。

另外，你需要知道網絡上有個MTU，對於以太網來說，MTU是1500字節，除去TCP+IP頭的40個字節，真正的數據傳輸可以有1460，這就是所謂的MSS（Max Segment Size）注意，TCP的RFC定義這個MSS的默認值是536，這是因為 RFC 791裡說了任何一個IP設備都得最少接收576尺寸的大小（實際上來說576是撥號的網絡的MTU，而576減去IP頭的20個字節就是536）。

如果你的網絡包可以塞滿MTU，那麼你可以用滿整個帶寬，如果不能，那麼你就會浪費帶寬。（大於MTU的包有兩種結局，一種是直接被丟了，另一種是會被重新分塊打包發送） 你可以想像成一個MTU就相當於一個飛機的最多可以裝的人，如果這飛機裡滿載的話，帶寬最高，如果一個飛機只運一個人的話，無疑成本增加了，也而相當二。

所以，Silly Windows Syndrome這個現像就像是你本來可以坐200人的飛機裡只做了一兩個人。 要解決這個問題也不難，就是避免對小的window size做出響應，直到有足夠大的window size再響應，這個思路可以同時實現在sender和receiver兩端。

如果這個問題是由Receiver端引起的，那麼就會使用 David D Clark’s 方案。在receiver端，如果收到的數據導致window size小於某個值，可以直接ack(0)回sender，這樣就把window給關閉了，也阻止了sender再發數據過來，等到receiver端處理了一些數據後windows size 大於等於了MSS，或者，receiver buffer有一半為空，就可以把window打開讓send 發送數據過來。

如果這個問題是由Sender端引起的，那麼就會使用著名的 Nagle’s algorithm。這個算法的思路也是延時處理，他有兩個主要的條件：1）要等到 Window Size>=MSS 或是 Data Size >=MSS，2）收到之前發送數據的ack回包，他才會發數據，否則就是在攢數據。

另外，Nagle算法默認是打開的，所以，對於一些需要小包場景的程序——比如像telnet或ssh這樣的交互性比較強的程序，你需要關閉這個算法。你可以在Socket設置TCP_NODELAY選項來關閉這個算法（關閉Nagle算法沒有全局參數，需要根據每個應用自己的特點來關閉）
1 setsockopt(sock_fd, IPPROTO_TCP, TCP_NODELAY, (char *)&value,sizeof(int));

另外，網上有些文章說TCP_CORK的socket option是也關閉Nagle算法，這不對。TCP_CORK其實是更新激進的Nagle算漢，完全禁止小包發送，而Nagle算法沒有禁止小包發送，只是禁止了大量的小包發送。最好不要兩個選項都設置。
-->

## 擁塞控制

TCP 通過 Sliding Window 來做流控 (Flow Control)，但是這種方式依賴於連接的發送端和接收端，並不知道網絡中間發生了什麼。

<!--
TCP的設計者覺得，一個偉大而牛逼的協議僅僅做到流控並不夠，因為流控只是網絡模型4層以上的事，TCP的還應該更聰明地知道整個網絡上的事。
-->

## 參考

關於 TCP/IP 協議可以參考 《TCP/IP 詳解 卷1：協議》，以及 RFC793 等一系列的標準，如下是一些比較經典的參考。

* [IP_TCP_UDP_Header 本地文檔](/reference/linux/network/IP_TCP_UDP_Header.pdf)，包括了 IP、TCP、UDP 頭部信息，以及 TCP 狀態轉換，也就是比較經典的格式。

* [網絡基本功](http://blog.csdn.net/xiongyingzhuantu/article/details/38665549) 一系列網路基本知識的介紹，包括了整個鏈路；[TCP 的那些事兒](http://coolshell.cn/articles/11564.html) 在 CoolShell 中的關於 TCP 協議的介紹，包括上述的圖片。

* [The TCP/IP Guide](htp://www.tcpipguide.com/index.htm) 很好的介紹 TCP 概念的資料，可以參考 [本地文檔](/reference/linux/The TCP-IP Guide - A Comprehensive Illustrated Internet Protocols Reference.pdf)，可以參考 TCP Fundamentals and General Operation 相關章節；[Nmap Network Scanning](http://nmap.org/book/tcpip-ref.html) 實際是 NMAP 的介紹，不過有很多相關協議的介紹。

* [高性能網絡編程](http://blog.csdn.net/column/details/high-perf-network.html) 包括了對網絡協議的詳細介紹。

<!--
http://blog.csdn.net/hc260164797/article/details/7563228
http://blog.csdn.net/hc260164797/article/details/7558624
http://blog.chinaunix.net/uid-29867135-id-4910871.html
http://blog.csdn.net/yusiguyuan/article/details/28625129
-->



{% highlight c %}
{% endhighlight %}
